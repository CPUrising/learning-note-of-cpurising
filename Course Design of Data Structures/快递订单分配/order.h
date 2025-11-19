#pragma once
#include <string>
#include <ctime>
#include <cmath>

using namespace std;

struct Coordinate {
    double x;
    double y;
};

enum class OrderStatus {
    PENDING,
    CANCELED,
    COMPLETED
};

struct Order {
    string orderId;
    Coordinate merchantPos;
    time_t requireArriveTime; // Unix timestamp
    double orderAmount;       // 元
    double avgCookTime;       // 分钟
    OrderStatus status;
    time_t lastUpdateTime;

    // 计算骑手到商家的欧氏距离（可替换为地图API距离）
    double calcDistanceToMerchant(const Coordinate& riderPos) const;

    // 计算优先级评分（值越小优先级越高）
    double calcPriorityScore(const Coordinate& riderPos, time_t currentTime) const;
};