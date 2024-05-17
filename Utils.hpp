#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <Windows.h>
#define _HIGH_RESOLUTION_DELAY_

void delay(size_t milliseconds);

class CursorInput {
public:
    CursorInput();

    void update(const int dx, const int dy);
    void update(const INPUT& newCursorInput);
    void show(const INPUT& cursorInput);
    void set_default();
    INPUT get();

private:
    std::atomic<INPUT> cursorInput;
    INPUT defaultCursorInput;
};

