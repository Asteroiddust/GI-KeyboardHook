#pragma once
#include <array>
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <Windows.h>

// 按键处理策略接口
class KeyHandler {
public:
    virtual void handle(int keyCode) = 0;
    virtual ~KeyHandler() {}
};

class KeyManager {
public:
    void dispatchKeyHandler(int keyCode);
    void registerKeyHandler(int keyCode, std::unique_ptr<KeyHandler> handler);
    inline bool getKey(int keyCode);
    void setKey(int keyCode, bool state);
private:
    std::map<int, std::unique_ptr<KeyHandler>> keyHandlers;
    std::atomic<bool> keyStates[256];
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
    virtual void handle(int keyCode) override;
};

// 处理F14按键的具体策略类
class Pick : public KeyHandler {
public:
    Pick();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 6> pick;
};

// 处理F15按键的具体策略类
class Click : public KeyHandler {
public:
    Click();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 2> click;
};

// 处理F16按键的具体策略类
class MachineGun : public KeyHandler {
public:
    MachineGun();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 5> machineGun;
};
