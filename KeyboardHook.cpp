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

// Keyboard hook call back function
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