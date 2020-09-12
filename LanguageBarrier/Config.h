#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdexcept>
#include <Shlwapi.h>
#include <ShlObj.h>
#include "data\defaultConfigJsonStr.h"
#include "lbjson.h"

namespace lb {
class Config {
  Config() = delete;
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;

 private:
  const std::wstring filename;
  explicit Config(const std::wstring& _filename)
      : filename(_filename) {
    j = json::parse(slurpFile(filename));
  }
  Config(const char* defaultStr, const std::wstring& pathEnd,
         REFKNOWNFOLDERID rfid)
      : filename(getPath(pathEnd, rfid)) {
    wchar_t dir[MAX_PATH];
    wcsncpy_s(dir, &filename[0], MAX_PATH);
    PathRemoveFileSpec(&dir[0]);
    SHCreateDirectoryEx(NULL, dir, NULL);
    load(defaultStr);
  }

  const std::wstring getPath(const std::wstring& pathEnd,
                             REFKNOWNFOLDERID rfid) {
    wchar_t* appdata;
    SHGetKnownFolderPath(rfid, NULL, NULL, &appdata);
    std::wstring result = std::wstring(appdata) + L"\\" + pathEnd;
    CoTaskMemFree(appdata);
    return result;
  }

  void load(const char* defaultStr) {
    json tmp1 = json::parse(defaultStr);
    json tmp2;
    try {
      tmp2 = json::parse(slurpFile(filename));
    } catch (...) {
    }
    j = json_merge(tmp1, tmp2);
    save();
  }

 public:
  static Config& config() {
    static Config s(defaultConfigJsonStr, L"Committee of Zero\\SGHD\\config.json",
                    FOLDERID_LocalAppData);
    return s;
  }
  static Config& sigs() {
    static Config s(L"languagebarrier\\signatures.json");
    return s;
  }
  static Config& fmv() {
    static Config s(L"languagebarrier\\fmv.json");
    return s;
  }
  static Config& fileredirection() {
    static Config s(L"languagebarrier\\fileredirection.json");
    return s;
  }
  static Config& stringredirection() {
    static Config s(L"languagebarrier\\stringredirection.json");
    return s;
  }

  json j;

  void save() {
    std::string s = j.dump(4);
    HANDLE h = CreateFileW(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
      DWORD written;
      WriteFile(h, s.data(), s.size(), &written, NULL);
      CloseHandle(h);
    }
  };
  static void init() {
    config();
    sigs();
    fmv();
    fileredirection();
    stringredirection();
  };
};
}

#endif  // !__CONFIG_H__
