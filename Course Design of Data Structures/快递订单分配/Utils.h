#pragma once
#include "Order.h"
#include <string>
#include <ctime>

using namespace std;

// 生成测试订单（方便在 main 中使用）
Order generateTestOrder(const string& orderId, time_t currentTime, int remainMinutes, double distanceFactor);