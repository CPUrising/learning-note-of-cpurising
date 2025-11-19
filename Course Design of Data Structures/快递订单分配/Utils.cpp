#include "Utils.h"
#include <cstdlib>

using namespace std;

Order generateTestOrder(const string& orderId, time_t currentTime, int remainMinutes, double distanceFactor) {
    Order o;
    o.orderId = orderId;
    // 这里用一个简单的坐标生成策略（可按工程需要改为真实坐标）
    o.merchantPos = { 116.40 + distanceFactor, 39.90 + distanceFactor };
    o.requireArriveTime = currentTime + remainMinutes * 60;
    o.orderAmount = 30.0 + (rand() % 30);   // 30 ~ 59
    o.avgCookTime = 3.0 + (rand() % 7);     // 3 ~ 9
    o.status = OrderStatus::PENDING;
    o.lastUpdateTime = currentTime;
    return o;
}
