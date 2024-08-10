#include "Config.hpp"
#include "KeyManager.hpp"
#include <format>
#include <iostream>
#include <thread>

namespace KeyManagement {

    std::atomic<bool> KeyManager::keyStates[256];
    std::unordered_map<int, std::unique_ptr<KeyManager>> KeyManager::keyManagerTable;

    void KeyManager::keyHandlerProc(int keyCode) {
        std::cout << "keyCode = " << keyCode << std::endl;
    }

    void KeyManager::dispatchKey(int keyCode) {
        auto managerQuery = keyManagerTable.find(keyCode);
        if (managerQuery != keyManagerTable.end()) {
            std::thread([&]() {
                managerQuery->second->keyHandlerProc(keyCode);
                }).detach();
        }
    }

    void KeyManager::registerKey(int keyCode, std::unique_ptr<KeyManager> manager) {
        keyManagerTable[keyCode] = std::move(manager);
    }

    bool KeyManager::getKeyState(int keyCode) {
        return keyStates[keyCode].load();
    }

    void KeyManager::setKeyState(int keyCode, bool state) {
        keyStates[keyCode].store(state);
    }

    constexpr INPUT Utils::createMouseInput(DWORD flags, LONG dx, LONG dy, DWORD mouseData) {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = flags;
        input.mi.dx = dx;
        input.mi.dy = dy;
        input.mi.mouseData = mouseData;
        return input;
    }

    constexpr INPUT Utils::createKeyboardInput(WORD key, DWORD flags) {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = key;
        input.ki.dwFlags = flags;
        return input;
    }

    const INPUT& Utils::getLeftDown() {
        static const INPUT leftDown = createMouseInput(MOUSEEVENTF_LEFTDOWN);
        return leftDown;
    }

    const INPUT& Utils::getLeftUp() {
        static const INPUT leftUp = createMouseInput(MOUSEEVENTF_LEFTUP);
        return leftUp;
    }

    const INPUT& Utils::getRDown() {
        static const INPUT rDown = createKeyboardInput('R');
        return rDown;
    }

    const INPUT& Utils::getRUp() {
        static const INPUT rUp = createKeyboardInput('R', KEYEVENTF_KEYUP);
        return rUp;
    }

    const INPUT& Utils::getFDown() {
        static const INPUT fDown = createKeyboardInput('F');
        return fDown;
    }

    const INPUT& Utils::getFUp() {
        static const INPUT fUp = createKeyboardInput('F', KEYEVENTF_KEYUP);
        return fUp;
    }

    const INPUT& Utils::getAntiDrift() {
        static const INPUT antiDrift = createMouseInput(MOUSEEVENTF_MOVE, 0, 3);
        return antiDrift;
    }

    void Utils::delay(double milliseconds) {
#ifdef HIGH_RESOLUTION_DELAY
        LARGE_INTEGER cur, target;
        QueryPerformanceCounter(&cur);
        target.QuadPart = cur.QuadPart + static_cast<LONGLONG>(milliseconds * 10000);
        while (cur.QuadPart < target.QuadPart) {
            QueryPerformanceCounter(&cur);
        }
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<LONGLONG>(milliseconds)));
#endif
    }

    void Utils::createBeepProcess(int frequency, int duration) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        wchar_t commandLine[100];
        swprintf(commandLine, 100, L"E:\\beep_process.exe %d %d", frequency, duration);

        if (!CreateProcess(
            NULL,
            commandLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        )) {
            std::cerr << "CreateProcess failed (" << GetLastError() << ")." << std::endl;
        }
        else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    bool Utils::SetProcessAffinity(DWORD processId, DWORD_PTR affinityMask) {
        HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION, FALSE, processId);
        if (hProcess == NULL) {
            std::cerr << "Failed to open process: " << GetLastError() << std::endl;
            return false;
        }
        if (!SetProcessAffinityMask(hProcess, affinityMask)) {
            std::cerr << "Failed to set process affinity: " << GetLastError() << std::endl;
            CloseHandle(hProcess);
            return false;
        }
        SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);
        CloseHandle(hProcess);
        return true;
    }

    const INPUT Spin::defaultInput = Utils::createMouseInput(MOUSEEVENTF_MOVE, DEFAULT_DX, DEFAULT_DY);
    std::atomic<INPUT> Spin::input{ Spin::defaultInput };

    void Spin::keyHandlerProc(int keyCode) {
        while (KeyManager::getKeyState(keyCode)) {
            auto freshInput = input.load();
            SendInput(1, &freshInput, sizeof(INPUT));
            Utils::delay(1);
        }
    }

    Spin::Spin() {
        input.store(defaultInput);
        show();
    }

    void Spin::update(const int dx, const int dy) {
        INPUT tempInput = input.load();
        tempInput.mi.dx += dx;
        tempInput.mi.dy += dy;
        input.store(tempInput);
        show();
    }

    void Spin::reset() {
        input.store(defaultInput);
        show();
    }

    auto Spin::get() {
        return input.load();
    }

    void Spin::show() {
        auto tempInput = get();
        std::cout << std::format("\rX = {: <5},Y = {: <5}", tempInput.mi.dx, -tempInput.mi.dy);
    }

    Pick::Pick() : pick{
        Utils::createKeyboardInput('F'),
        Utils::createKeyboardInput('F', KEYEVENTF_KEYUP),
        Utils::createMouseInput(MOUSEEVENTF_WHEEL, 0, 0, -120),
        Utils::createKeyboardInput('F'),
        Utils::createKeyboardInput('F', KEYEVENTF_KEYUP),
        Utils::createMouseInput(MOUSEEVENTF_WHEEL, 0, 0, 120)
    } {}

    void Pick::keyHandlerProc(int keyCode) {
        static size_t i = pick.size() - 1;
        while (KeyManager::getKeyState(keyCode)) {
            i = (i + 1) % pick.size();
            SendInput(1, const_cast<INPUT*>(&pick[i]), sizeof(INPUT));
            Utils::delay(10);
        }
        i = pick.size() - 1;
    }

    void Click::keyHandlerProc(int keyCode) {
        while (KeyManager::getKeyState(keyCode)) {
            SendInput(1, const_cast<INPUT*>(&Utils::getLeftDown()), sizeof(INPUT));
            SendInput(1, const_cast<INPUT*>(&Utils::getLeftUp()), sizeof(INPUT));
            Utils::delay(10);
        }
    }

    void MachineGun::keyHandlerProc(int keyCode) {        
#ifdef ANTI_DRIFT
        static INPUT antiDrift = Utils::getAntiDrift();
#endif // ANTI_DRIFT
        while (KeyManager::getKeyState(keyCode)) {
            SendInput(1, const_cast<INPUT*>(&Utils::getRDown()), sizeof(INPUT));
            Utils::delay(KEY_PRESS_GAP);
            SendInput(1, const_cast<INPUT*>(&Utils::getLeftDown()), sizeof(INPUT));
            SendInput(1, const_cast<INPUT*>(&Utils::getRUp()), sizeof(INPUT));
            Utils::delay(KEY_PRESS_GAP - 0.2);
#ifdef ANTI_DRIFT // 循环中连续小回正
            if (antiDrift.mi.dx <= MAX_ANTIDRIFT_DX / KEY_PRESS_GAP) {
                antiDrift.mi.dx += ANTIDRIFT_DX_STEP;
            }
            SendInput(1, &antiDrift, sizeof(INPUT));
#endif // ANTI_DRIFT
            SendInput(1, const_cast<INPUT*>(&Utils::getRDown()), sizeof(INPUT));
            SendInput(1, const_cast<INPUT*>(&Utils::getLeftUp()), sizeof(INPUT));
            Utils::delay(KEY_PRESS_GAP);
            SendInput(1, const_cast<INPUT*>(&Utils::getRUp()), sizeof(INPUT));
            Utils::delay(KEY_PRESS_GAP);
        }
#ifdef ANTI_DRIFT // 循环结束收尾大回正
        antiDrift.mi.dx = COEF_SHIFT_BACK * antiDrift.mi.dx;
        SendInput(1, &antiDrift, sizeof(INPUT));
        antiDrift.mi.dx = 0;
#endif // ANTI_DRIFT
    }
} // namespace KeyManagement