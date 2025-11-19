#include "Order.h"
#include <algorithm>

using namespace std;

double Order::calcDistanceToMerchant(const Coordinate& riderPos) const {
    double dx = riderPos.x - merchantPos.x;
    double dy = riderPos.y - merchantPos.y;
    return sqrt(dx * dx + dy * dy);
}

double Order::calcPriorityScore(const Coordinate& riderPos, time_t currentTime) const {
    // 1e18 表示极高的评分（最低优先级）
    if (status != OrderStatus::PENDING) return 1e18;

    double remainTime = difftime(requireArriveTime, currentTime) / 60.0; // 分钟
    if (remainTime <= 0) return 0.0; // 已超时 -> 最高优先级

    double distance = calcDistanceToMerchant(riderPos);
    double amountFactor = (orderAmount > 0.0) ? (1.0 / orderAmount) : 0.1;
    double cookTime = avgCookTime;

    // 权重可调整：剩余时间 0.4，距离 0.3，金额反比 0.2，出餐时间 0.1
    return (remainTime * 0.4) + (distance * 0.3) + (amountFactor * 0.2) + (cookTime * 0.1);
}
