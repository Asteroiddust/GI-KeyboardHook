#include "Utils.hpp"
#include <format>
#include <iostream>
#include <Windows.h>

using namespace std::chrono_literals;

void delay(size_t miliseconds) {
    if (miliseconds >= 15) {
        miliseconds -= 15;
        std::this_thread::sleep_for(1ms);
    }
    LARGE_INTEGER cur, target;
    QueryPerformanceCounter(&cur);
    target.QuadPart = cur.QuadPart + miliseconds * 10000;
    while (cur.QuadPart < target.QuadPart) {
        QueryPerformanceCounter(&cur);
    }
}

CursorInput::CursorInput() {
    defaultCursorInput.type = INPUT_MOUSE;
#ifdef _HIGH_RESOLUTION_DELAY_
    defaultCursorInput.mi.dx = 100;       // ��� x ��ƫ����
    defaultCursorInput.mi.dy = 5;         // ��� y ��ƫ����
#else
    defaultCursorInput.mi.dx = 150;       // ��� x ��ƫ����
    defaultCursorInput.mi.dy = 10;        // ��� y ��ƫ����
#endif // _HIGH_RESOLUTION_DELAY_
    defaultCursorInput.mi.mouseData = 0;      // ��������ƶ���ͨ������Ϊ 0   
    defaultCursorInput.mi.dwFlags = MOUSEEVENTF_MOVE; // ���Ϊ����ƶ��¼�
    defaultCursorInput.mi.time = 0;           // ����Ϊ 0����ϵͳ�Զ����ʱ���
    defaultCursorInput.mi.dwExtraInfo = NULL; // ͨ������Ϊ NULL
    update(defaultCursorInput);
}

void CursorInput::update(const int dx, const int dy) {
    INPUT input = cursorInput.load();
    input.mi.dx += dx;
    input.mi.dy += dy;
    cursorInput.store(input);
    show(input);
}

void CursorInput::update(const INPUT& newCursorInput) {
    cursorInput.store(newCursorInput);
    show(newCursorInput);
}

void CursorInput::show(const INPUT& cursorInput) {
    std::cout << std::format("\rX = {: >4}, Y = {: >4}", cursorInput.mi.dx, -cursorInput.mi.dy);
}

void CursorInput::set_default() {
    update(defaultCursorInput);
}

INPUT CursorInput::get() {
    return cursorInput.load();
}


