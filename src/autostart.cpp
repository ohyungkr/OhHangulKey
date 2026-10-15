#include "autostart.h"
#include <cwchar>

static const wchar_t* REG_RUN_KEY = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
static const wchar_t* APP_KEY_NAME = L"OhHangulKey";

bool IsAutoStartEnabled() {
    HKEY hKey = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    wchar_t buffer[MAX_PATH * 2] = { 0 };
    DWORD bufferSize = sizeof(buffer);
    DWORD type = 0;

    LONG result = RegQueryValueExW(hKey, APP_KEY_NAME, NULL, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize);
    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS && type == REG_SZ && wcslen(buffer) > 0);
}

bool SetAutoStart(bool enable) {
    HKEY hKey = NULL;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    bool success = false;
    if (enable) {
        wchar_t exePath[MAX_PATH] = { 0 };
        GetModuleFileNameW(NULL, exePath, MAX_PATH);

        // 따옴표로 감싸서 경로에 공백이 있어도 안전하게 실행되도록 구성
        wchar_t quotedPath[MAX_PATH + 4] = { 0 };
        swprintf_s(quotedPath, L"\"%s\"", exePath);

        DWORD byteLen = static_cast<DWORD>((wcslen(quotedPath) + 1) * sizeof(wchar_t));
        LONG result = RegSetValueExW(hKey, APP_KEY_NAME, 0, REG_SZ, reinterpret_cast<const BYTE*>(quotedPath), byteLen);
        success = (result == ERROR_SUCCESS);
    } else {
        LONG result = RegDeleteValueW(hKey, APP_KEY_NAME);
        success = (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND);
    }

    RegCloseKey(hKey);
    return success;
}
