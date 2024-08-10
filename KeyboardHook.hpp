#pragma once
#include <functional>
#include <unordered_map>
#include <Windows.h>

// 预先计算的常量值
const std::unordered_map<int, std::function<void()>> keyHandlers;

// Keyboard hook call back function
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);