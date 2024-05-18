#include "KeyHandler.hpp"
#include "Utils.hpp"

extern CursorInput dragonSpin;
extern KeyManager keyManager;

// 定义F13按键处理策略
void Spin::handle(int keyCode) {
    while (keyManager.getKey(keyCode)) {
        auto ret = dragonSpin.get();
        SendInput(1, &ret, sizeof(INPUT));
#ifdef _HIGH_RESOLUTION_DELAY_
        delay(1);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif // _HIGH_RESOLUTION_DELAY_
    }
}

// 定义F14按键处理策略
Pick::Pick() {
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

void Pick::handle(int keyCode) {
    while (keyManager.getKey(keyCode)) {
        static size_t i = pick.size() - 1;
        i = (i + 1) % pick.size();
        SendInput(1, &pick[i], sizeof(INPUT));
#ifdef _HIGH_RESOLUTION_DELAY_
        delay(10);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif // _HIGH_RESOLUTION_DELAY_
    }
}

// 定义F15按键处理策略
Click::Click() {
    click.fill(INPUT());
    click[0].type = INPUT_MOUSE;
    click[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    click[1].type = INPUT_MOUSE;
    click[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
}

void Click::handle(int keyCode) {
    while (keyManager.getKey(keyCode)) {
        SendInput(1, &click[0], sizeof(INPUT));
        SendInput(1, &click[1], sizeof(INPUT));
#ifdef _HIGH_RESOLUTION_DELAY_
        delay(20);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
#endif // _HIGH_RESOLUTION_DELAY_
    }
}

// 定义F16按键处理策略
MachineGun::MachineGun() {
    machineGun.fill(INPUT());
    machineGun[0].type = INPUT_MOUSE;
    machineGun[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    machineGun[1].type = INPUT_MOUSE;
    machineGun[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    machineGun[2].type = INPUT_KEYBOARD;
    machineGun[2].ki.wVk = 'R';   
    machineGun[3].type = INPUT_KEYBOARD;
    machineGun[3].ki.wVk = 'R';
    machineGun[3].ki.dwFlags = KEYEVENTF_KEYUP;
    machineGun[4].type = INPUT_MOUSE;
#ifdef _HIGH_RESOLUTION_DELAY_
    machineGun[4].mi.dx = 80;
    machineGun[4].mi.dy = 3;
#else
    defaultCursorInput.mi.dx = 150;
#endif // _HIGH_RESOLUTION_DELAY_ 
    machineGun[4].mi.dwFlags = MOUSEEVENTF_MOVE;
}

void MachineGun::handle(int keyCode) {
    auto& leftDown  = machineGun[0];
    auto& leftUp    = machineGun[1];
    auto& rDown     = machineGun[2];
    auto& rUp       = machineGun[3];
    auto& antiDrift = machineGun[4];
    size_t cnt = 0;
    while (keyManager.getKey(keyCode)) {
        SendInput(1, &rDown, sizeof(INPUT));
        delay(9.5);
        SendInput(1, &leftDown, sizeof(INPUT));
        SendInput(1, &rUp, sizeof(INPUT));
        delay(9.5);
        if (++cnt >= 5) {
            SendInput(1, &antiDrift, sizeof(INPUT));
        }        
        SendInput(1, &rDown, sizeof(INPUT));
        SendInput(1, &leftUp, sizeof(INPUT));
        delay(9.5);
        SendInput(1, &rUp, sizeof(INPUT));
        delay(9.5);        
    }
}

void KeyManager::dispatchKeyHandler(int keyCode) {
    auto handler = keyHandlers.find(keyCode);
    if (handler != keyHandlers.end()) {
        std::thread([&, handler, keyCode]() {
            handler->second->handle(keyCode);
            }).detach();
    }
}

inline bool KeyManager::getKey(int keyCode) {
    return keyStates[keyCode].load();
}

void KeyManager::setKey(int keyCode, bool state) {
    keyStates[keyCode].store(state);
}

void KeyManager::registerKeyHandler(int keyCode, std::unique_ptr<KeyHandler> handler) {
    keyHandlers[keyCode] = std::move(handler);
}
