#ifndef __LANGUAGEBARRIER_H__
#define __LANGUAGEBARRIER_H__

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdint>
#include <cstdlib>
#include <wchar.h>
#include <string>
#include "MinHook.h"

namespace lb {
size_t alignCeil(size_t val, size_t align);
std::string slurpFile(const std::string &fileName);
std::string slurpFile(const std::wstring &fileName);
std::string wideToUTF8(const wchar_t* data, size_t size);
inline std::string wideToUTF8(const wchar_t* data) {
  return wideToUTF8(data, wcslen(data));
}
void LanguageBarrierInit();
void LanguageBarrierLog(const std::string &text);
bool scanCreateEnableHook(char *category, char *name, uintptr_t *ppTarget,
                          LPVOID pDetour, LPVOID *ppOriginal);
bool createEnableApiHook(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour,
                         LPVOID *ppOriginal);
// until TsudaKageyu updates his NuGet package...
MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour,
                        LPVOID *ppOriginal, LPVOID *ppTarget);
}

#endif  // !__LANGUAGEBARRIER_H__
