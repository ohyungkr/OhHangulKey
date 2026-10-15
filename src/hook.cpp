#include "hook.h"
#include "config.h"
#include "resource.h"

static HHOOK g_hHook = NULL;
static HWND g_hTrayWnd = NULL;

void SetTrayWindowHandle(HWND hWnd) {
    g_hTrayWnd = hWnd;
}

// 가상 키보드 입력 주입 함수
static void SendKeyWithModifierRelease(WORD modifierVk, WORD targetVk) {
    INPUT inputs[4] = { 0 };

    // 1. 조합 중인 모디파이어 키 임시 해제
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = modifierVk;
    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    // 2. 대상 키 누름 (한/영: VK_HANGUL, 한자: VK_HANJA)
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = targetVk;
    inputs[1].ki.dwFlags = 0;

    // 3. 대상 키 뗌
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = targetVk;
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

    // 4. 원래 누르고 있던 모디파이어 키 다시 누름 상태로 복원
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = modifierVk;
    inputs[3].ki.dwFlags = 0;

    SendInput(4, inputs, sizeof(INPUT));
}

// 저수준 키보드 후킹 콜백 프로시저
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* pKey = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

    // 프로그램 또는 타 프로그램이 주입한 가상 키 이벤트는 루프 방지를 위해 가로채지 않고 통과
    if (pKey->flags & LLKHF_INJECTED) {
        return CallNextHookEx(g_hHook, nCode, wParam, lParam);
    }

    static bool s_spaceDown = false;
    static bool s_interceptedSpace = false;

    bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

    if (pKey->vkCode == VK_SPACE) {
        if (isKeyDown) {
            // 모디파이어 키 상태 비동기 확인 (좌/우 키 개별 확인 포함)
            bool shiftPressed = ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_RSHIFT) & 0x8000) != 0);

            bool ctrlPressed  = ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_LCONTROL) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0);

            bool altPressed   = ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_LMENU) & 0x8000) != 0) ||
                                ((GetAsyncKeyState(VK_RMENU) & 0x8000) != 0) ||
                                ((pKey->flags & LLKHF_ALTDOWN) != 0);

            // 1. Ctrl + Shift + Space: 전체 기능 ON/OFF 토글
            if (g_config.ctrlShiftSpaceToggle && ctrlPressed && shiftPressed && !altPressed) {
                s_interceptedSpace = true;
                // 키 리피트 방지: 처음 눌렸을 때만 1회 토글 메시지 발송
                if (!s_spaceDown) {
                    s_spaceDown = true;
                    if (g_hTrayWnd) {
                        PostMessageW(g_hTrayWnd, WM_TOGGLE_STATUS, 0, 0);
                    }
                }
                return 1; // 원본 Space 키 이벤트 차단
            }

            // 전체 기능이 켜져 있는 상태에서만 Shift+Space 및 Ctrl+Space 변환 처리
            if (g_config.isEnabled) {
                // 2. Shift + Space: 우측 Alt (한/영 전환)
                if (g_config.shiftSpaceToRAlt && shiftPressed && !ctrlPressed && !altPressed) {
                    s_interceptedSpace = true;
                    if (!s_spaceDown) {
                        s_spaceDown = true;
                        SendKeyWithModifierRelease(VK_SHIFT, VK_HANGUL);
                    }
                    return 1; // 원본 Space 키 이벤트 차단
                }

                // 3. Ctrl + Space: 우측 Ctrl (한자 변환)
                if (g_config.ctrlSpaceToRCtrl && ctrlPressed && !shiftPressed && !altPressed) {
                    s_interceptedSpace = true;
                    if (!s_spaceDown) {
                        s_spaceDown = true;
                        SendKeyWithModifierRelease(VK_CONTROL, VK_HANJA);
                    }
                    return 1; // 원본 Space 키 이벤트 차단
                }
            }
        } else if (isKeyUp) {
            s_spaceDown = false;
            if (s_interceptedSpace) {
                s_interceptedSpace = false;
                return 1; // 가로챈 조합의 Space KeyUp 이벤트도 소비
            }
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

bool InstallHook(HINSTANCE hInstance) {
    if (g_hHook) return true;

    g_hHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);
    return (g_hHook != NULL);
}

void UninstallHook() {
    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
}
