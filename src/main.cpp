#include <windows.h>
#include "resource.h"
#include "config.h"
#include "tray.h"
#include "hook.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR /*lpCmdLine*/, int /*nCmdShow*/) {
    // 중복 실행 방지를 위한 명명된 뮤텍스
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"Local\\OhHangulKey_SingleInstance_Mutex");
    if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
        if (hMutex) {
            CloseHandle(hMutex);
        }
        MessageBoxW(NULL, L"OhHangulKey가 이미 실행 중입니다.\n작업 표시줄 트레이 아이콘을 확인하세요.", L"OhHangulKey", MB_OK | MB_ICONINFORMATION);
        return 0;
    }

    // 설정 파일 읽기
    LoadConfig();

    // 트레이 아이콘 및 윈도우 초기화
    if (!InitTray(hInstance)) {
        CloseHandle(hMutex);
        return 1;
    }

    // 키보드 후킹에 트레이 윈도우 핸들 등록
    SetTrayWindowHandle(GetTrayWindow());

    // 전역 저수준 키보드 후킹 설치
    if (!InstallHook(hInstance)) {
        MessageBoxW(NULL, L"키보드 후킹 등록에 실패했습니다.", L"OhHangulKey 오류", MB_OK | MB_ICONERROR);
        CleanupTray();
        CloseHandle(hMutex);
        return 1;
    }

    // 트레이 아이콘 초기 상태 갱신
    UpdateTrayIcon();

    // 표준 Win32 메시지 루프 (초경량 이벤트 기반 대기)
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // 종료 정리
    UninstallHook();
    CleanupTray();

    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }

    return 0;
}
