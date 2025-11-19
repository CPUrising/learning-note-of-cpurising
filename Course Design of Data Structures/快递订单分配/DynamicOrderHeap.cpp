#include "DynamicOrderHeap.h"
#include <cstdio>
#include <cmath>

using namespace std;

void DynamicOrderHeap::swapNode(int i, int j) {
    Order tmp = heap[i];
    heap[i] = heap[j];
    heap[j] = tmp;
    orderIndex[heap[i].orderId] = i;
    orderIndex[heap[j].orderId] = j;
}

void DynamicOrderHeap::siftUp(int idx, const Coordinate& riderPos, time_t currentTime) {
    while (idx > 0) {
        int parentIdx = (idx - 1) / 2;
        double childScore = heap[idx].calcPriorityScore(riderPos, currentTime);
        double parentScore = heap[parentIdx].calcPriorityScore(riderPos, currentTime);
        if (childScore < parentScore - EPS) {
            swapNode(idx, parentIdx);
            idx = parentIdx;
        }
        else {
            break;
        }
    }
}

void DynamicOrderHeap::siftDown(int idx, const Coordinate& riderPos, time_t currentTime) {
    int n = (int)heap.size();
    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;

        if (left < n) {
            double leftScore = heap[left].calcPriorityScore(riderPos, currentTime);
            double smallestScore = heap[smallest].calcPriorityScore(riderPos, currentTime);
            if (leftScore < smallestScore - EPS) smallest = left;
        }
        if (right < n) {
            double rightScore = heap[right].calcPriorityScore(riderPos, currentTime);
            double smallestScore = heap[smallest].calcPriorityScore(riderPos, currentTime);
            if (rightScore < smallestScore - EPS) smallest = right;
        }

        if (smallest != idx) {
            swapNode(idx, smallest);
            idx = smallest;
        }
        else {
            break;
        }
    }
}

bool DynamicOrderHeap::pushOrder(const Order& newOrder, const Coordinate& riderPos, time_t currentTime) {
    if (orderIndex.find(newOrder.orderId) != orderIndex.end()) {
        cout << "错误：订单 " << newOrder.orderId << " 已存在！" << endl;
        return false;
    }
    Order o = newOrder;
    o.lastUpdateTime = currentTime;
    orderIndex[o.orderId] = (int)heap.size();
    heap.push_back(o);
    siftUp((int)heap.size() - 1, riderPos, currentTime);
    cout << "成功添加订单：" << o.orderId << endl;
    return true;
}

Order DynamicOrderHeap::getTopOrder(const Coordinate& riderPos, time_t currentTime) {
    // 清理堆顶的无效订单
    while (!heap.empty()) {
        const Order& top = heap[0];
        if (top.status == OrderStatus::PENDING) {
            return top;
        }
        else {
            popTopOrder(riderPos, currentTime);
        }
    }
    throw runtime_error("无待配送订单！");
}

void DynamicOrderHeap::popTopOrder(const Coordinate& riderPos, time_t currentTime) {
    if (heap.empty()) {
        cout << "警告：堆为空，无需删除！" << endl;
        return;
    }
    string topId = heap[0].orderId;
    orderIndex.erase(topId);
    int lastIdx = (int)heap.size() - 1;
    if (lastIdx > 0) {
        heap[0] = heap[lastIdx];
    }
    heap.pop_back();
    if (!heap.empty()) {
        orderIndex[heap[0].orderId] = 0;
        siftDown(0, riderPos, currentTime);
    }
    cout << "成功移除订单（开始配送）：" << topId << endl;
}

bool DynamicOrderHeap::cancelOrder(const string& orderId, const Coordinate& riderPos, time_t currentTime) {
    auto it = orderIndex.find(orderId);
    if (it == orderIndex.end()) {
        cout << "错误：订单 " << orderId << " 不存在！" << endl;
        return false;
    }
    int idx = it->second;
    heap[idx].status = OrderStatus::CANCELED;
    heap[idx].lastUpdateTime = currentTime;
    // 将其下沉以降低优先级
    siftDown(idx, riderPos, currentTime);
    cout << "成功取消订单：" << orderId << endl;
    return true;
}

void DynamicOrderHeap::updateAllPriorities(const Coordinate& riderPos, time_t currentTime) {
    cout << "\n=== 开始更新所有订单优先级 ===" << endl;
    for (int i = 0; i < (int)heap.size(); ++i) {
        Order& order = heap[i];
        if (order.status != OrderStatus::PENDING) continue;
        double oldScore = order.calcPriorityScore(riderPos, order.lastUpdateTime);
        double newScore = order.calcPriorityScore(riderPos, currentTime);
        if (fabs(newScore - oldScore) > 0.5) {
            order.lastUpdateTime = currentTime;
            // 双向调整，确保堆属性
            siftUp(i, riderPos, currentTime);
            siftDown(i, riderPos, currentTime);
            cout << "订单" << order.orderId << "优先级更新：旧评分" << oldScore << " → 新评分" << newScore << endl;
        }
    }
    cout << "=== 优先级更新完成 ===" << endl;
}

void DynamicOrderHeap::printPendingOrders(const Coordinate& riderPos, time_t currentTime) const {
    cout << "\n当前待配送订单列表（按堆顺序）：" << endl;
    cout << "索引\t订单ID\t状态\t剩余时间(分钟)\t优先级评分" << endl;
    for (int i = 0; i < (int)heap.size(); ++i) {
        const Order& order = heap[i];
        string statusStr = (order.status == OrderStatus::PENDING) ? "待配送" :
            (order.status == OrderStatus::CANCELED) ? "已取消" : "已完成";
        double remainTime = difftime(order.requireArriveTime, currentTime) / 60.0;
        double score = order.calcPriorityScore(riderPos, currentTime);
        printf("%d\t%s\t%s\t%.1f\t\t\t%.6f\n", i, order.orderId.c_str(), statusStr.c_str(), remainTime, score);
    }
}

bool DynamicOrderHeap::isEmpty() const {
    return heap.empty();
}
