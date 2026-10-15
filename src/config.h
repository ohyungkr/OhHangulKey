#pragma once
#include <windows.h>

// 애플리케이션 설정 구조체
struct AppConfig {
    bool shiftSpaceToRAlt;      // Shift + Space -> 우측 Alt (한/영) 동작 여부
    bool ctrlSpaceToRCtrl;       // Ctrl + Space  -> 우측 Ctrl (한자) 동작 여부
    bool ctrlShiftSpaceToggle;   // Ctrl + Shift + Space -> 전체 기능 ON/OFF 토글 동작 여부
    bool isEnabled;              // 전체 매크로 기능 활성화 상태
};

// 전역 설정 인스턴스 참조
extern AppConfig g_config;

// 설정 파일 경로 반환 (실행 파일 동일 디렉터리의 OhHangulKey.ini)
void GetConfigFilePath(wchar_t* outPath, size_t maxLen);

// 설정 불러오기
void LoadConfig();

// 설정 저장하기
void SaveConfig();
