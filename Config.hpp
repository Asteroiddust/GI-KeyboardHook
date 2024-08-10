#pragma once
#define HIGH_RESOLUTION_DELAY // 启用Utils::Delay()函数中的高精度延时
#define ANTI_DRIFT

constexpr double KEY_PRESS_GAP = 8;

#ifdef HIGH_RESOLUTION_DELAY
constexpr int DEFAULT_DX = 100;
constexpr int DEFAULT_DY = 5;
#else
#include <chrono>
constexpr int DEFAULT_DX = 150;
constexpr int DEFAULT_DY = 10;
#endif // HIGH_RESOLUTION_DELAY


#ifdef ANTI_DRIFT
constexpr long MAX_ANTIDRIFT_DX = 650;
constexpr long ANTIDRIFT_DX_STEP = 4;
constexpr long COEF_SHIFT_BACK = -10;
#endif // ANTI_DRIFT