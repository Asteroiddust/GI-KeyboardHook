#pragma once
#include <functional>
#include <unordered_map>
#include <Windows.h>

// Ԥ�ȼ���ĳ���ֵ
const std::unordered_map<int, std::function<void()>> keyHandlers;

// Keyboard hook call back function
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);