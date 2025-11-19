#include "Order.h"
#include "DynamicOrderHeap.h"
#include "Utils.h"
#include <iostream>
#include <ctime>

using namespace std;

int main() {
    srand((unsigned int)time(nullptr));
    DynamicOrderHeap orderHeap;
    Coordinate riderPos = { 116.405, 39.903 };
    time_t currentTime = time(nullptr);

    cout << "【场景1：初始添加3个订单】" << endl;
    orderHeap.pushOrder(generateTestOrder("O001", currentTime, 10, 0.01), riderPos, currentTime);
    orderHeap.pushOrder(generateTestOrder("O002", currentTime, 5, 0.02), riderPos, currentTime);
    orderHeap.pushOrder(generateTestOrder("O003", currentTime, 8, 0.005), riderPos, currentTime);
    orderHeap.printPendingOrders(riderPos, currentTime);

    cout << "\n【场景2：获取并开始配送最优订单】" << endl;
    try {
        Order top1 = orderHeap.getTopOrder(riderPos, currentTime);
        cout << "当前最优订单：" << top1.orderId << "（剩余时间" << difftime(top1.requireArriveTime, currentTime) / 60.0 << "分钟）" << endl;
        orderHeap.popTopOrder(riderPos, currentTime);
        orderHeap.printPendingOrders(riderPos, currentTime);
    }
    catch (const exception& e) {
        cerr << "异常: " << e.what() << endl;
    }

    cout << "\n【场景3：5分钟后，新增订单+骑手位置变化】" << endl;
    currentTime += 5 * 60;
    riderPos = { 116.408, 39.906 };
    orderHeap.pushOrder(generateTestOrder("O004", currentTime, 8, 0.003), riderPos, currentTime);
    orderHeap.updateAllPriorities(riderPos, currentTime);
    orderHeap.printPendingOrders(riderPos, currentTime);

    cout << "\n【场景4：取消订单O003】" << endl;
    orderHeap.cancelOrder("O003", riderPos, currentTime);
    orderHeap.printPendingOrders(riderPos, currentTime);

    cout << "\n【场景5：获取新的最优订单】" << endl;
    try {
        Order top2 = orderHeap.getTopOrder(riderPos, currentTime);
        cout << "当前最优订单：" << top2.orderId << "（剩余时间" << difftime(top2.requireArriveTime, currentTime) / 60.0 << "分钟）" << endl;
        orderHeap.popTopOrder(riderPos, currentTime);
        orderHeap.printPendingOrders(riderPos, currentTime);
    }
    catch (const exception& e) {
        cerr << "异常: " << e.what() << endl;
    }

    return 0;
}
