#include "KeyBoardHook.hpp"
#include "KeyManager.hpp"
#include <basetsd.h>
#include <consoleapi3.h>
#include <errhandlingapi.h>
#include <iostream>
#include <memory>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <WinBase.h>
#include <windef.h>
#include <WinUser.h>

using namespace KeyManagement;

int main() {
    // Find game window
    HWND hGameWnd = FindWindow(L"UnityWndClass", NULL);
    if (hGameWnd == NULL) {
        std::cerr << "Failed to find game! (" << GetLastError() << ")" << std::endl;
        return 1;
    }

    // Attach game process to Pcores of 13600KF (assuming Hyper-Threading is enabled) 
    DWORD pidGame;
    DWORD_PTR PerformanceCoresMask = 0xFFF;
    GetWindowThreadProcessId(hGameWnd, &pidGame);
    if (!Utils::SetProcessAffinity(pidGame, PerformanceCoresMask)) {
        std::cerr << "Failed to set game process affinity! (" << GetLastError() << ")" << std::endl;
        return 2;
    }

    // Install the keyboard hook
    HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (hKeyboardHook == NULL) {
        std::cerr << "Failed to install keyboard hook! (" << GetLastError() << ")" << std::endl;
        return 3;
    }

    // Attach tool process to Ecores of 13600KF
    DWORD_PTR EfficientCoresMask = 0xFF000;
    if (!SetProcessAffinityMask(GetCurrentProcess(), EfficientCoresMask)) {
        std::cerr << "Failed to set tool process affinity! (" << GetLastError() << ")" << std::endl;
        return 4;
    }

    // Make sure this tool is DPI awared and hide its window
    SetProcessDPIAware();
    ShowWindow(GetConsoleWindow(), SW_MINIMIZE);

    // Set game window foreground
    ShowWindow(hGameWnd, SW_RESTORE);
    SetForegroundWindow(hGameWnd);

    // Initialize key managers
    KeyManager::registerKey(VK_F13, std::make_unique<MachineGun>());
    KeyManager::registerKey(VK_F14, std::make_unique<Pick>());
    KeyManager::registerKey(VK_F15, std::make_unique<Click>());
    KeyManager::registerKey(VK_F16, std::make_unique<Spin>());

    // Beep to notice user that the program is initialized
    Utils::createBeepProcess(750, 300);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the keyboard hook before exiting
    UnhookWindowsHookEx(hKeyboardHook);

    // Beep to notice user that the program is done
    Utils::createBeepProcess(375, 300);
    return 0;
}
