#pragma once
#include <array>
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <Windows.h>
#include "Utils.hpp"

extern CursorInput dragonSpin;

// 按键处理策略接口
class KeyHandler {
public:
    virtual void handle(int keyCode) = 0;
    virtual ~KeyHandler() {}
};

class KeyManager {
public:
    void dispatchKeyHandler(int keyCode) {
        auto handler = keyHandlers.find(keyCode);
        if (handler != keyHandlers.end()) {
            std::thread([&]() {
                handler->second->handle(keyCode);
                }).detach();
        }
    }

    void registerKeyHandler(int keyCode, std::unique_ptr<KeyHandler> handler) {
        keyHandlers[keyCode] = std::move(handler);
    }

    static auto getKey(int keyCode) {
        return keyStates[keyCode].load();
    }

    static void setKey(int keyCode, bool state) {
        keyStates[keyCode].store(state);
    }
private:
    std::map<int, std::unique_ptr<KeyHandler>> keyHandlers;
    static std::atomic<bool> keyStates[256];
};

class DefaultHandler : public KeyHandler {
public:
    virtual void handle(int keyCode) override {
        std::cout << "keyCode " << keyCode << std::endl;
    }
};

// 处理F13按键的具体策略类
class Spin : public KeyHandler {
public:
    virtual void handle(int keyCode) override {
        while (KeyManager::getKey(keyCode)) {
            auto ret = dragonSpin.get();
            SendInput(1, &ret, sizeof(INPUT));
            delay(1);
        }
    }
};

// 处理F14按键的具体策略类
class Pick : public KeyHandler {
public:
    Pick() {
        pick.fill(INPUT());
        pick[0].type = INPUT_KEYBOARD;
        pick[0].ki.wVk = 'F';
        pick[1].type = INPUT_KEYBOARD;
        pick[1].ki.wVk = 'F';
        pick[1].ki.dwFlags = KEYEVENTF_KEYUP;
        pick[2].type = INPUT_MOUSE;
        pick[2].mi.dwFlags = MOUSEEVENTF_WHEEL;
        pick[2].mi.mouseData = -120;
        pick[3].type = INPUT_KEYBOARD;
        pick[3].ki.wVk = 'F';
        pick[4].type = INPUT_KEYBOARD;
        pick[4].ki.wVk = 'F';
        pick[4].ki.dwFlags = KEYEVENTF_KEYUP;
        pick[5].type = INPUT_MOUSE;
        pick[5].mi.dwFlags = MOUSEEVENTF_WHEEL;
        pick[5].mi.mouseData = 120;
    }

    virtual void handle(int keyCode) override {
        while (KeyManager::getKey(keyCode)) {
            static size_t i = pick.size() - 1;
            i = (i + 1) % pick.size();
            SendInput(1, &pick[i], sizeof(INPUT));
            delay(10);
        }
    }
private:
    std::array<INPUT, 6> pick;
};

// 处理F15按键的具体策略类
class Click : public KeyHandler {
public:
    Click() {
        click.fill(INPUT());
        click[0].type = INPUT_MOUSE;
        click[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        click[1].type = INPUT_MOUSE;
        click[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    }

    virtual void handle(int keyCode) override {
        while (KeyManager::getKey(keyCode)) {
            SendInput(1, &click[0], sizeof(INPUT));
            SendInput(1, &click[1], sizeof(INPUT));
            delay(10);
        }
    }
private:
    std::array<INPUT, 2> click;
};

// 处理F16按键的具体策略类
#define ANTI_DRIFT
class MachineGun : public KeyHandler {
public:
    MachineGun() :
        leftDown(createLeftDown()),
        leftUp(createLeftUp()),
        rDown(createKeyDown()),
        rUp(createKeyUp()),
        antiDrift(createMouseMove()) {}

    virtual void handle(int keyCode) override {
        constexpr double duration = 8;
        while (KeyManager::getKey(keyCode)) {
            SendInput(1, &rDown, sizeof(INPUT));
            delay(duration);
            SendInput(1, &leftDown, sizeof(INPUT));
            SendInput(1, &rUp, sizeof(INPUT));
            delay(duration - 0.2);
#ifdef ANTI_DRIFT
            if (antiDrift.mi.dx <= 650 / duration) {
                antiDrift.mi.dx += 4;
            }
            SendInput(1, &antiDrift, sizeof(INPUT));
#endif
            SendInput(1, &rDown, sizeof(INPUT));
            SendInput(1, &leftUp, sizeof(INPUT));
            delay(duration);
            SendInput(1, &rUp, sizeof(INPUT));
            delay(duration);
        }
#ifdef ANTI_DRIFT
        antiDrift.mi.dx = -10 * antiDrift.mi.dx;
        SendInput(1, &antiDrift, sizeof(INPUT));
        antiDrift.mi.dx = 0;
#endif
    }
private:
    INPUT leftDown;
    INPUT leftUp;
    INPUT rDown;
    INPUT rUp;
    INPUT antiDrift;
    
    static constexpr INPUT createLeftDown() {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        return input;
    }
    static constexpr INPUT createLeftUp() {
        INPUT input{};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        return input;
    }
    static constexpr INPUT createKeyDown() {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 'R';
        return input;
    }
    static constexpr INPUT createKeyUp() {
        INPUT input{};
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = 'R';
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        return input;
    }
    static constexpr INPUT createMouseMove() {
        INPUT input{};
        input.type = INPUT_MOUSE;
#ifdef HIGH_RESOLUTION_DELAY
        input.mi.dx = 0;
        input.mi.dy = 3;
#else
        input.mi.dx = 80;
#endif // _HIGH_RESOLUTION_DELAY_ 
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        return input;
    }
};

// 处理F17按键的具体策略类
class HideCursor : public KeyHandler {
public:
    HideCursor() {
        x = GetSystemMetrics(SM_CXSCREEN);
        y = GetSystemMetrics(SM_CYSCREEN);
    }
    virtual void handle(int keyCode) override {
        while (KeyManager::getKey(keyCode)) {
            SetCursorPos(x, y);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
private:
    int x, y;
};


