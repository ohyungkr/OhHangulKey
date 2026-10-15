#include "config.h"
#include <shlwapi.h>

// 기본 설정값 초기화 (모두 활성화)
AppConfig g_config = {
    true, // shiftSpaceToRAlt
    true, // ctrlSpaceToRCtrl
    true, // ctrlShiftSpaceToggle
    true  // isEnabled
};

void GetConfigFilePath(wchar_t* outPath, size_t maxLen) {
    if (!outPath || maxLen == 0) return;

    // 현재 실행 모듈 경로 조회
    GetModuleFileNameW(NULL, outPath, static_cast<DWORD>(maxLen));

    // 파일명 부분을 OhHangulKey.ini 로 변경
    wchar_t* lastSlash = wcsrchr(outPath, L'\\');
    if (lastSlash) {
        *(lastSlash + 1) = L'\0';
        wcscat_s(outPath, maxLen, L"OhHangulKey.ini");
    } else {
        wcscpy_s(outPath, maxLen, L"OhHangulKey.ini");
    }
}

void LoadConfig() {
    wchar_t iniPath[MAX_PATH] = { 0 };
    GetConfigFilePath(iniPath, MAX_PATH);

    // INI 파일에서 설정값 읽기 (기본값: 1)
    g_config.shiftSpaceToRAlt = (GetPrivateProfileIntW(L"Settings", L"ShiftSpaceToRAlt", 1, iniPath) != 0);
    g_config.ctrlSpaceToRCtrl  = (GetPrivateProfileIntW(L"Settings", L"CtrlSpaceToRCtrl", 1, iniPath) != 0);
    
    // CtrlShiftSpaceToggle 우선 읽고, 없으면 구버전 AltSpaceToggle도 확인
    int toggleVal = GetPrivateProfileIntW(L"Settings", L"CtrlShiftSpaceToggle", -1, iniPath);
    if (toggleVal == -1) {
        toggleVal = GetPrivateProfileIntW(L"Settings", L"AltSpaceToggle", 1, iniPath);
    }
    g_config.ctrlShiftSpaceToggle = (toggleVal != 0);

    g_config.isEnabled         = (GetPrivateProfileIntW(L"Settings", L"Enabled", 1, iniPath) != 0);
}

void SaveConfig() {
    wchar_t iniPath[MAX_PATH] = { 0 };
    GetConfigFilePath(iniPath, MAX_PATH);

    // INI 파일에 설정값 저장
    WritePrivateProfileStringW(L"Settings", L"ShiftSpaceToRAlt", g_config.shiftSpaceToRAlt ? L"1" : L"0", iniPath);
    WritePrivateProfileStringW(L"Settings", L"CtrlSpaceToRCtrl",  g_config.ctrlSpaceToRCtrl ? L"1" : L"0", iniPath);
    WritePrivateProfileStringW(L"Settings", L"CtrlShiftSpaceToggle", g_config.ctrlShiftSpaceToggle ? L"1" : L"0", iniPath);
    WritePrivateProfileStringW(L"Settings", L"Enabled",           g_config.isEnabled ? L"1" : L"0", iniPath);
}
