#include "LanguageBarrier.h"
#include <ctime>
#include "MinHook.h"
#include "Config.h"
#include "Game.h"
#include "SigScan.h"

static bool isInitialised = false;

namespace lb {
size_t alignCeil(size_t val, size_t align) {
  return (val % align == 0) ? val : val + align - (val % align);
}
std::string slurpFileCommon(HANDLE h) {
  if (h == INVALID_HANDLE_VALUE)
    throw std::runtime_error("cannot read file");
  DWORD size = GetFileSize(h, NULL);
  std::string result(size, 0);
  DWORD read;
  if (!ReadFile(h, &result[0], size, &read, NULL) || read != size) {
    CloseHandle(h);
    throw std::runtime_error("cannot read file");
  }
  CloseHandle(h);
  return result;
}
std::string slurpFile(const std::string &fileName) {
  HANDLE h = CreateFileA(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  return slurpFileCommon(h);
}
std::string slurpFile(const std::wstring &fileName) {
  HANDLE h = CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  return slurpFileCommon(h);
}
std::string wideToUTF8(const wchar_t* data, size_t size) {
  std::string result(size * 4, 0);
  int actualSize = WideCharToMultiByte(CP_UTF8, 0, data, size, &result[0], result.size(), NULL, NULL);
  result.resize(actualSize);
  return result;
}
void LanguageBarrierInit() {
  if (isInitialised) {
    LanguageBarrierLog("LanguageBarrierInit() called twice...");
    return;
  }
  isInitialised = true;

  std::remove("languagebarrier\\log.txt");
  // TODO: proper versioning
  LanguageBarrierLog("LanguageBarrier for STEINS;GATE v1.03");
  LanguageBarrierLog("**** Start apprication ****");

  MH_STATUS mhStatus = MH_Initialize();
  if (mhStatus != MH_OK) {
    LanguageBarrierLog(std::string("MinHook failed to initialize!") + MH_StatusToString(mhStatus));
    return;
  }

  Config::init();

  WCHAR path[MAX_PATH], exeName[_MAX_FNAME];
  GetModuleFileNameW(NULL, path, MAX_PATH);
  _wsplitpath_s(path, NULL, 0, NULL, 0, exeName, _MAX_FNAME, NULL, 0);
  if (_wcsicmp(exeName, L"Launcher") != 0) {
    {
      LanguageBarrierLog("Game.exe detected");
    }
    gameInit();
  }
}
// TODO: make this better
void LanguageBarrierLog(const std::string &text) {
  HANDLE h = CreateFileA("languagebarrier\\log.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (h == INVALID_HANDLE_VALUE)
    return;
  SetFilePointer(h, 0, NULL, FILE_END);
  SYSTEMTIME time;
  GetSystemTime(&time);
  char buffer[64];
  sprintf(buffer, "[%02d/%02d/%02d %02d:%02d:%02d] ",
    time.wMonth, time.wDay, time.wYear % 100,
    time.wHour, time.wMinute, time.wSecond);
  std::string result = std::string(buffer) + text + "\r\n";
  DWORD written;
  WriteFile(h, result.data(), result.size(), &written, NULL);
  CloseHandle(h);
}
bool scanCreateEnableHook(char *category, char *name, uintptr_t *ppTarget,
                          LPVOID pDetour, LPVOID *ppOriginal) {
  *ppTarget = sigScan(category, name);
  if (*ppTarget == NULL) return false;

  MH_STATUS mhStatus;
  mhStatus = MH_CreateHook((LPVOID)*ppTarget, pDetour, ppOriginal);
  if (mhStatus != MH_OK) {
    LanguageBarrierLog(std::string("Failed to create hook ") + name + ": " + MH_StatusToString(mhStatus));
    return false;
  }
  mhStatus = MH_EnableHook((LPVOID)*ppTarget);
  if (mhStatus != MH_OK) {
    LanguageBarrierLog(std::string("Failed to enable hook ") + name + ": " + MH_StatusToString(mhStatus));
    return false;
  }

  LanguageBarrierLog(std::string("Successfully hooked ") + name);

  return true;
}
bool createEnableApiHook(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour,
                         LPVOID *ppOriginal) {
  MH_STATUS mhStatus;
  LPVOID pTarget;
  mhStatus =
      MH_CreateHookApiEx(pszModule, pszProcName, pDetour, ppOriginal, &pTarget);
  if (mhStatus != MH_OK) {
    LanguageBarrierLog("Failed to create API hook " + wideToUTF8(pszModule) +
      "." + pszProcName + ": " + MH_StatusToString(mhStatus));
    return false;
  }
  mhStatus = MH_EnableHook(pTarget);
  if (mhStatus != MH_OK) {
    LanguageBarrierLog("Failed to enable API hook " + wideToUTF8(pszModule) +
      "." + pszProcName + ": " + MH_StatusToString(mhStatus));
    return false;
  }

  LanguageBarrierLog("Successfully hooked " + wideToUTF8(pszModule) + "." + pszProcName);

  return true;
}
//-------------------------------------------------------------------------
MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName,
                             LPVOID pDetour, LPVOID *ppOriginal,
                             LPVOID *ppTarget) {
  HMODULE hModule;
  LPVOID pTarget;

  hModule = GetModuleHandleW(pszModule);
  if (hModule == NULL) return MH_ERROR_MODULE_NOT_FOUND;

  pTarget = (LPVOID)GetProcAddress(hModule, pszProcName);
  if (pTarget == NULL) return MH_ERROR_FUNCTION_NOT_FOUND;

  if (ppTarget != NULL) *ppTarget = pTarget;

  return MH_CreateHook(pTarget, pDetour, ppOriginal);
}
}
