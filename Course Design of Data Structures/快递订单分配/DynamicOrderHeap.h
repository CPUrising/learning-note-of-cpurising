#pragma once
#include "Order.h"
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

using namespace std;

class DynamicOrderHeap {
private:
    vector<Order> heap;
    unordered_map<string, int> orderIndex;
    const double EPS = 1e-6;

    void swapNode(int i, int j);
    void siftUp(int idx, const Coordinate& riderPos, time_t currentTime);
    void siftDown(int idx, const Coordinate& riderPos, time_t currentTime);

public:
    DynamicOrderHeap() = default;
    ~DynamicOrderHeap() = default;

    bool pushOrder(const Order& newOrder, const Coordinate& riderPos, time_t currentTime);
    Order getTopOrder(const Coordinate& riderPos, time_t currentTime);
    void popTopOrder(const Coordinate& riderPos, time_t currentTime);
    bool cancelOrder(const string& orderId, const Coordinate& riderPos, time_t currentTime);
    void updateAllPriorities(const Coordinate& riderPos, time_t currentTime);
    void printPendingOrders(const Coordinate& riderPos, time_t currentTime) const;
    bool isEmpty() const;
};