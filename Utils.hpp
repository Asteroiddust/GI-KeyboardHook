#pragma once
#include <atomic>
#include <chrono>
#include <thread>
#include <Windows.h>
#define HIGH_RESOLUTION_DELAY

using namespace std::chrono_literals;

void delay(double milliseconds) {
#ifdef HIGH_RESOLUTION_DELAY
    LARGE_INTEGER cur, target;
    QueryPerformanceCounter(&cur);
    target.QuadPart = cur.QuadPart + static_cast<LONGLONG>(milliseconds * 10000);
    while (cur.QuadPart < target.QuadPart) {
        QueryPerformanceCounter(&cur);
    }
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(1) * milliseconds);
#endif
}

class CursorInput {
public:
    CursorInput() {
        defaultCursorInput.type = INPUT_MOUSE;
#ifdef HIGH_RESOLUTION_DELAY
        defaultCursorInput.mi.dx = 100;       // 相对 x 轴偏移量
        defaultCursorInput.mi.dy = 5;         // 相对 y 轴偏移量
#else
        defaultCursorInput.mi.dx = 150;       // 相对 x 轴偏移量
        defaultCursorInput.mi.dy = 10;        // 相对 y 轴偏移量
#endif // _HIGH_RESOLUTION_DELAY_
        defaultCursorInput.mi.mouseData = 0;      // 对于鼠标移动，通常设置为 0   
        defaultCursorInput.mi.dwFlags = MOUSEEVENTF_MOVE; // 标记为鼠标移动事件
        defaultCursorInput.mi.time = 0;           // 设置为 0，让系统自动填充时间戳
        defaultCursorInput.mi.dwExtraInfo = NULL; // 通常设置为 NULL
        update(defaultCursorInput);
    }

    void update(const int dx, const int dy) {
        INPUT input = cursorInput.load();
        input.mi.dx += dx;
        input.mi.dy += dy;
        cursorInput.store(input);
        show(input);
    }

    void update(const INPUT& newCursorInput) {
        cursorInput.store(newCursorInput);
        show(newCursorInput);
    }

    void set_default() {
        update(defaultCursorInput);
    }

    INPUT get() {
        return cursorInput.load();
    }

private:
    std::atomic<INPUT> cursorInput;
    INPUT defaultCursorInput;

    void show(const INPUT& cursorInput) {
        std::cout << std::format("\rX = {: <5},Y = {: <5}", cursorInput.mi.dx, -cursorInput.mi.dy);
    }
};

