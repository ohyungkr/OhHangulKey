#include "tray.h"
#include "resource.h"
#include "config.h"
#include "autostart.h"
#include <shellapi.h>

static HWND g_hWnd = NULL;
static NOTIFYICONDATAW g_nid = { 0 };
static HINSTANCE g_hInstance = NULL;
static const wchar_t* WND_CLASS_NAME = L"OhHangulKey_TrayWndClass";

static void ShowTrayContextMenu(HWND hWnd) {
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    // 1. Shift + Space
    UINT flagShift = MF_STRING | (g_config.shiftSpaceToRAlt ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flagShift, IDM_SHIFT_SPACE, L"Shift + Space - 우측 Alt (한글/영문)");

    // 2. Ctrl + Space
    UINT flagCtrl = MF_STRING | (g_config.ctrlSpaceToRCtrl ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flagCtrl, IDM_CTRL_SPACE, L"Ctrl + Space  - 우측 Ctrl (한자)");

    // 3. Ctrl + Shift + Space
    UINT flagToggle = MF_STRING | (g_config.ctrlShiftSpaceToggle ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flagToggle, IDM_CTRL_SHIFT_SPACE, L"Ctrl + Shift + Space - 전체 기능 ON/OFF 토글");

    // 4. 윈도우 시작 시 자동 실행
    bool isAutoStart = IsAutoStartEnabled();
    UINT flagAuto = MF_STRING | (isAutoStart ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flagAuto, IDM_AUTOSTART, L"윈도우 시작시에 프로그램 실행");

    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

    // 현재 활성화 상태 표시 및 즉시 토글 메뉴
    const wchar_t* statusText = g_config.isEnabled ? L"전체 기능: 동작 중 (ON)" : L"전체 기능: 일시 정지 (OFF)";
    UINT flagStatus = MF_STRING | (g_config.isEnabled ? MF_CHECKED : MF_UNCHECKED);
    AppendMenuW(hMenu, flagStatus, IDM_STATUS_TOGGLE, statusText);

    AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"OhHangulKey 정보(&A)...");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"종료(&X)");

    // 트레이 컨텍스트 메뉴가 다른 곳 클릭 시 정상적으로 닫히도록 설정
    SetForegroundWindow(hWnd);
    TrackPopupMenuEx(hMenu, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, hWnd, NULL);
    PostMessageW(hWnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}

void UpdateTrayIcon(bool showBalloon) {
    if (!g_hWnd) return;

    // 상태에 따른 아이콘 및 툴팁 전환
    int iconId = g_config.isEnabled ? IDI_APP_ICON : IDI_APP_ICON_OFF;
    if (g_nid.hIcon) {
        DestroyIcon(g_nid.hIcon);
    }
    g_nid.hIcon = (HICON)LoadImageW(g_hInstance, MAKEINTRESOURCEW(iconId), IMAGE_ICON,
                                    GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    if (g_config.isEnabled) {
        wcscpy_s(g_nid.szTip, L"OhHangulKey (동작 중)\nShift+Space: 한/영 | Ctrl+Space: 한자");
    } else {
        wcscpy_s(g_nid.szTip, L"OhHangulKey (일시 정지됨)\nCtrl+Shift+Space로 다시 활성화할 수 있습니다.");
    }

    if (showBalloon) {
        g_nid.uFlags |= NIF_INFO;
        wcscpy_s(g_nid.szInfoTitle, L"OhHangulKey");
        if (g_config.isEnabled) {
            wcscpy_s(g_nid.szInfo, L"전체 매크로 기능이 [활성화] 되었습니다.");
            g_nid.dwInfoFlags = NIIF_INFO;
        } else {
            wcscpy_s(g_nid.szInfo, L"전체 매크로 기능이 [일시 정지] 되었습니다.");
            g_nid.dwInfoFlags = NIIF_WARNING;
        }
    }

    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

void ToggleStatus(bool showBalloon) {
    g_config.isEnabled = !g_config.isEnabled;
    SaveConfig();
    UpdateTrayIcon(showBalloon);
}

static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TOGGLE_STATUS:
        ToggleStatus(true);
        return 0;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP || lParam == WM_CONTEXTMENU) {
            ShowTrayContextMenu(hWnd);
        } else if (lParam == WM_LBUTTONUP) {
            // 좌클릭 시에도 편리하게 팝업 메뉴를 띄움
            ShowTrayContextMenu(hWnd);
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_SHIFT_SPACE:
            g_config.shiftSpaceToRAlt = !g_config.shiftSpaceToRAlt;
            SaveConfig();
            break;

        case IDM_CTRL_SPACE:
            g_config.ctrlSpaceToRCtrl = !g_config.ctrlSpaceToRCtrl;
            SaveConfig();
            break;

        case IDM_CTRL_SHIFT_SPACE:
            g_config.ctrlShiftSpaceToggle = !g_config.ctrlShiftSpaceToggle;
            SaveConfig();
            break;

        case IDM_AUTOSTART: {
            bool current = IsAutoStartEnabled();
            SetAutoStart(!current);
            break;
        }

        case IDM_STATUS_TOGGLE:
            ToggleStatus(false);
            break;

        case IDM_ABOUT:
            MessageBoxW(hWnd,
                L"OhHangulKey v1.0.0\n\n"
                L"우측 Alt/Ctrl이 없는 키보드를 위한 초경량 전역 키보드 매핑 유틸리티입니다.\n\n"
                L"단축키 안내:\n"
                L" - Shift + Space        : 우측 Alt (한/영 전환)\n"
                L" - Ctrl + Space         : 우측 Ctrl (한자 변환)\n"
                L" - Ctrl + Shift + Space : 전체 기능 ON/OFF 토글\n\n"
                L"무설치/의존성 없는 독립 실행 파일입니다.",
                L"OhHangulKey 정보",
                MB_OK | MB_ICONINFORMATION);
            break;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool InitTray(HINSTANCE hInstance) {
    g_hInstance = hInstance;

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WND_CLASS_NAME;

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    g_hWnd = CreateWindowExW(
        0,
        WND_CLASS_NAME,
        L"OhHangulKeyTrayWindow",
        WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, hInstance, NULL
    );

    if (!g_hWnd) {
        return false;
    }

    ZeroMemory(&g_nid, sizeof(g_nid));
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_nid.uCallbackMessage = WM_TRAYICON;

    int iconId = g_config.isEnabled ? IDI_APP_ICON : IDI_APP_ICON_OFF;
    g_nid.hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(iconId), IMAGE_ICON,
                                    GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
    wcscpy_s(g_nid.szTip, L"OhHangulKey (동작 중)");

    Shell_NotifyIconW(NIM_ADD, &g_nid);
    return true;
}

void CleanupTray() {
    if (g_nid.hWnd) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        if (g_nid.hIcon) {
            DestroyIcon(g_nid.hIcon);
            g_nid.hIcon = NULL;
        }
        g_nid.hWnd = NULL;
    }

    if (g_hWnd) {
        DestroyWindow(g_hWnd);
        g_hWnd = NULL;
    }

    if (g_hInstance) {
        UnregisterClassW(WND_CLASS_NAME, g_hInstance);
        g_hInstance = NULL;
    }
}

HWND GetTrayWindow() {
    return g_hWnd;
}
