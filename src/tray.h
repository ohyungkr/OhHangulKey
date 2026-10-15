#pragma once
#include <windows.h>

// 트레이 서브시스템 초기화 및 메시지 윈도우 생성
bool InitTray(HINSTANCE hInstance);

// 트레이 아이콘 및 리소스 정리
void CleanupTray();

// 트레이 아이콘 및 툴팁 갱신
void UpdateTrayIcon(bool showBalloon = false);

// 전체 기능 활성화 상태 토글 (단일 진입점)
void ToggleStatus(bool showBalloon = false);

// 트레이 윈도우 핸들 반환
HWND GetTrayWindow();
