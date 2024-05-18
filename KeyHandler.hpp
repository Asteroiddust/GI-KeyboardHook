#pragma once
#include <array>
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <Windows.h>

// ����������Խӿ�
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

// ����F13�����ľ��������
class Spin : public KeyHandler {
public:
    virtual void handle(int keyCode) override;
};

// ����F14�����ľ��������
class Pick : public KeyHandler {
public:
    Pick();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 6> pick;
};

// ����F15�����ľ��������
class Click : public KeyHandler {
public:
    Click();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 2> click;
};

// ����F16�����ľ��������
class MachineGun : public KeyHandler {
public:
    MachineGun();
    virtual void handle(int keyCode) override;
private:
    std::array<INPUT, 5> machineGun;
};
