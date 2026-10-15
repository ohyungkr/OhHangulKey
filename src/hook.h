#pragma once
#include <windows.h>

// 키보드 후킹 초기화 및 설치
bool InstallHook(HINSTANCE hInstance);

// 키보드 후킹 해제
void UninstallHook();

// 트레이 윈도우 핸들 등록 (상태 변경 통지용)
void SetTrayWindowHandle(HWND hWnd);
