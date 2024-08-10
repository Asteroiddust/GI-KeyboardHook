#include "KeyHandler.hpp"
#include "Utils.hpp"
#include <iostream>
#include <Windows.h>
#include <memory>

std::atomic<bool> KeyManager::keyStates[256];

CursorMoveInput neuvilletteSpin;
KeyManager keyManager;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
    int keyCode = keyInfo->vkCode;
    switch (wParam) {
    case WM_KEYDOWN: {
        switch (keyCode)
        {
        case VK_F12:
            PostQuitMessage(0);
            break;
        case VK_UP:
            neuvilletteSpin.update(0, -5);
            break;
        case VK_DOWN:
            neuvilletteSpin.update(0, 5);
            break;
        case VK_LEFT:
            dragonSpin.update(-10, 0);
            break;
        case VK_RIGHT:
            dragonSpin.update(10, 0);
            break;
        case VK_HOME:
            dragonSpin.set_default();
            break;
        default:
            if (!KeyManager::getKey(keyCode)) {
                KeyManager::setKey(keyCode, true);
                keyManager.dispatchKeyHandler(keyCode);
            }
            break;
        }
    }
        break;
    case WM_KEYUP:
        KeyManager::setKey(keyCode, false);
        break;
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Install the keyboard hook
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (hKeyboardHook == NULL) {
        std::cerr << "Failed to install keyboard hook! (" << GetLastError() << ")" << std::endl;
        return 3;
    }

    // Set Genshin window foreground
        ShowWindow(hGenshinWnd, SW_RESTORE);
        SetForegroundWindow(hGenshinWnd);
    }

    // Adjust process DPI awareness and priority level
    SetProcessDPIAware();
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

    // Initialize key handlers
    keyManager.registerKeyHandler(VK_F13, std::make_unique<MachineGun>());
    keyManager.registerKeyHandler(VK_F14, std::make_unique<Pick>());
    keyManager.registerKeyHandler(VK_F15, std::make_unique<Click>());
    keyManager.registerKeyHandler(VK_F16, std::make_unique<Spin>());
    keyManager.registerKeyHandler(VK_F17, std::make_unique<HideCursor>());

    // Beep to remind user that the program is initialized
    Beep(750, 300);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the keyboard hook before exiting
    UnhookWindowsHookEx(hKeyboardHook);

    // Beep to remind user that the program is terminated
    Beep(375, 300);
    return 0;
}
