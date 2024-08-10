#include "KeyManager.hpp"
#include <functional>
#include <minwindef.h>
#include <unordered_map>
#include <WinUser.h>

using namespace KeyManagement;

// 预先计算的常量值
const std::unordered_map<int, std::function<void()>> keyHandlers = {
    { VK_UP, []() { Spin::update(0, -5); } },
    { VK_DOWN, []() { Spin::update(0, 5); } },
    { VK_LEFT, []() { Spin::update(-10, 0); } },
    { VK_RIGHT, []() { Spin::update(10, 0); } },
    { VK_HOME, []() {
        Spin::reset();
        Utils::createBeepProcess(750, 300);
    } }
};

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
    int keyCode = keyInfo->vkCode;
    switch (wParam)
    {
    case WM_KEYDOWN:
        // When key is pressed
        switch (keyCode)
        {
        case VK_F12:    // F12 - Exit program            
            PostQuitMessage(0);
            break;
        default:
            // 使用预先计算的常量值
            if (auto it = keyHandlers.find(keyCode); it != keyHandlers.end()) {
                it->second();
            }
            else if (!KeyManager::getKeyState(keyCode)) {
                // Avoid retriggering
                KeyManager::setKeyState(keyCode, true);
                KeyManager::dispatchKey(keyCode);
        }
    }
        break;
    case WM_KEYUP:
        KeyManager::setKeyState(keyCode, false);
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
