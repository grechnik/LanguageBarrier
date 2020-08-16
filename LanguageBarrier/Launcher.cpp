#include "Config.h"
#include "Launcher.h"
#include "LanguageBarrier.h"
#include <Windows.h>
#include <map>
#include <fstream>

typedef BOOL(__stdcall* SETWINDOWTEXTW)(HWND, LPCWSTR);
typedef HWND(__stdcall* CREATEDIALOGINDIRECTPARAMW)(HINSTANCE, LPCDLGTEMPLATEW, HWND, DLGPROC, LPARAM);
typedef	LRESULT(__stdcall* SENDMESSAGE)(HWND, UINT, WPARAM, LPARAM);
static SETWINDOWTEXTW SetWindowTextOrig = NULL;
static CREATEDIALOGINDIRECTPARAMW CreateDialogIndirectParamOrig = NULL;
static SENDMESSAGE SendMessageOrig = NULL;

extern json rawConfig;

HANDLE logFile;
static DLGPROC origDlgProc; // mages launcher uses the same proc for all dialogs
static HFONT newFont = NULL;
static bool firstDialogShown = false;
static std::string* patchSettingsDialog = NULL;
static std::wstring patchSettingsButtonName;
static std::map<std::wstring, std::wstring> textReplacements;

// should not conflict with ids from mages launcher, currently 1000..1003
#define IDC_PATCH_SETTINGS_BUTTON 1432

// a button with this ID in the dialog from settingsDialog.res,
// if present, is treated as reset-to-defaults
#define IDC_RESET_DEFAULTS_BUTTON 999

namespace lb {

std::wstring utf8ToWide(const std::string& data) {
  int convertedCount = MultiByteToWideChar(CP_UTF8, 0, data.data(), data.size(), NULL, 0);
  std::wstring result;
  result.resize(convertedCount);
  MultiByteToWideChar(CP_UTF8, 0, data.data(), data.size(), &result[0], convertedCount);
  return result;
}

void initPatchSettings(HWND hWnd, json& settings) {
  json& controls = config["patch"]["launcher"]["patchDialog"]["controls"];
  for (auto it = controls.begin(); it != controls.end(); ++it) {
    std::string key = it.key();
    if (settings[key].is_boolean() && it->is_number_integer()) {
      SendDlgItemMessage(hWnd, it->get<int>(), BM_SETCHECK, settings[key].get<bool>(), 0);
    } else if (settings[key].is_string() && it->is_object()) {
      std::string current = settings[key].get<std::string>();
      for (auto jt = it->begin(); jt != it->end(); ++jt) {
        if (jt->is_number_integer()) {
          SendDlgItemMessage(hWnd, jt->get<int>(), BM_SETCHECK, current == jt.key(), 0);
        }
      }
    }
  }
}

void updatePatchSettings(HWND hWnd) {
  extern std::wstring configGetPath();
  json& controls = config["patch"]["launcher"]["patchDialog"]["controls"];
  for (auto it = controls.begin(); it != controls.end(); ++it) {
    std::string key = it.key();
    if (rawConfig[key].is_boolean() && it->is_number_integer()) {
      LRESULT state = SendDlgItemMessage(hWnd, it->get<int>(), BM_GETCHECK, 0, 0);
      rawConfig[key] = (bool)state;
    } else if (rawConfig[key].is_string() && it->is_object()) {
      for (auto jt = it->begin(); jt != it->end(); ++jt) {
        if (jt->is_number_integer()) {
          LRESULT state = SendDlgItemMessage(hWnd, jt->get<int>(), BM_GETCHECK, 0, 0);
          if (state) {
            rawConfig[key] = jt.key();
            break;
          }
        }
      }
    }
  }
  std::wstring path = configGetPath();
  std::ofstream o(path);
  o << rawConfig.dump(2) << '\n';
}

INT_PTR __stdcall patchSettingsDlgFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
  case WM_INITDIALOG:
    initPatchSettings(hWnd, rawConfig);
    return TRUE;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDOK:
      updatePatchSettings(hWnd);
      EndDialog(hWnd, 1);
      return TRUE;
    case IDCANCEL:
      EndDialog(hWnd, 0);
      return TRUE;
	case IDC_RESET_DEFAULTS_BUTTON:
	  {
        std::ifstream i("languagebarrier\\defaultconfig.json");
        json defaultconfig;
        i >> defaultconfig;
        initPatchSettings(hWnd, defaultconfig);
      }
	  return TRUE;
    }
  }
  return FALSE;
}

BOOL __stdcall SetWindowTextHook(HWND hWnd, LPCWSTR str) {
  auto it = textReplacements.find(str);
  return SetWindowTextOrig(hWnd, it == textReplacements.end() ? str : it->second.c_str());
}

LRESULT __stdcall SendMessageHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == CB_INSERTSTRING) {
    auto it = textReplacements.find((const wchar_t*)lParam);
    if (it != textReplacements.end())
      lParam = (LPARAM)it->second.c_str();
  }
  return SendMessageOrig(hWnd, msg, wParam, lParam);
}

BOOL __stdcall setCustomFont(HWND hWnd, LPARAM) {
  SendMessage(hWnd, WM_SETFONT, (WPARAM)newFont, 0);
  return TRUE;
}

INT_PTR __stdcall DlgFuncHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if (msg == WM_INITDIALOG) {
    if (newFont)
      EnumChildWindows(hWnd, setCustomFont, 0);
    if (!firstDialogShown && !patchSettingsButtonName.empty()) {
      RECT button = { 9, 50, 100, 50 + 26 };
      MapDialogRect(hWnd, &button);
      HWND hButton = CreateWindowExW(0, L"Button", patchSettingsButtonName.c_str(),
                                     WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                                     button.left, button.top,
                                     button.right - button.left, button.bottom - button.top,
                                     hWnd, (HMENU)IDC_PATCH_SETTINGS_BUTTON, NULL, NULL);
      HFONT dlgfont = newFont ? newFont : (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
      SendMessage(hButton, WM_SETFONT, (WPARAM)dlgfont, 0);
      firstDialogShown = true;
    }
  } else if (msg == WM_COMMAND && wParam == IDC_PATCH_SETTINGS_BUTTON) {
    if (patchSettingsDialog) {
      // assume one dialog and nothing else in the resource file, then the header is 0x40 bytes
      const DLGTEMPLATE* dlg = (const DLGTEMPLATE*)(patchSettingsDialog->data() + 0x40);
      DialogBoxIndirectParam(NULL, dlg, hWnd, patchSettingsDlgFunc, 0);
    }
    return TRUE;
  }
  return origDlgProc(hWnd, msg, wParam, lParam);
}

HWND __stdcall CreateDialogIndirectParamHook(HINSTANCE hInstance, LPCDLGTEMPLATEW lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam) {
  origDlgProc = lpDialogFunc;
  return CreateDialogIndirectParamOrig(hInstance, lpTemplate, hWndParent, DlgFuncHook, dwInitParam);
}

void initFont(json& fontCfg) {
  if (!fontCfg.is_object()) // no "font" section in config
    return;
  const json& typeface = fontCfg["typeface"];
  const json& size = fontCfg["size"];
  if (!typeface.is_string() || !size.is_number_integer())
    return;
  LOGFONTW logfont;
  memset(&logfont, 0, sizeof(logfont));
  HDC screenDC = GetDC(NULL);
  logfont.lfHeight = -MulDiv(size.get<int>(), GetDeviceCaps(screenDC, LOGPIXELSY), 72);
  ReleaseDC(NULL, screenDC);
  logfont.lfCharSet = DEFAULT_CHARSET;
  MultiByteToWideChar(CP_UTF8,
    0,
    typeface.get<std::string>().c_str(),
    -1,
    logfont.lfFaceName,
    LF_FACESIZE);
  newFont = CreateFontIndirectW(&logfont);
}

void initPatchDialog(json& dialogCfg) {
  if (!dialogCfg.is_object())
    return;
  const json& buttonName = dialogCfg["buttonName"];
  if (!buttonName.is_string() || !dialogCfg["controls"].is_object())
    return;
  slurpFile("languagebarrier\\settingsDialog.res", &patchSettingsDialog);
  patchSettingsButtonName = utf8ToWide(buttonName.get<std::string>());
}

void initTextReplacements(json& cfg) {
  if (!cfg.is_object())
    return;
  for (auto it = cfg.begin(); it != cfg.end(); ++it) {
    if (!it->is_string())
      continue;
    textReplacements[utf8ToWide(it.key())] = utf8ToWide(it->get<std::string>());
  }
}

void enhanceLauncher() {
  json& launcherCfg = config["patch"]["launcher"];
  if (!launcherCfg.is_object()) // no "launcher" section in config
    return;
  initFont(launcherCfg["font"]);
  initPatchDialog(launcherCfg["patchDialog"]);
  initTextReplacements(launcherCfg["textReplacements"]);
  MH_Initialize();
  createEnableApiHook(L"user32", "SetWindowTextW", &SetWindowTextHook, (LPVOID*)&SetWindowTextOrig);
  createEnableApiHook(L"user32", "CreateDialogIndirectParamW", &CreateDialogIndirectParamHook, (LPVOID*)&CreateDialogIndirectParamOrig);
  createEnableApiHook(L"user32", "SendMessageW", &SendMessageHook, (LPVOID*)&SendMessageOrig);
}
} // namespace lb
