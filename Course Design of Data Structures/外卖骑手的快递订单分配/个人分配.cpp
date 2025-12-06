#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <ctime>
#include <string>
#include <unordered_map>
#include <random>
using namespace std;

// 坐标结构（改为公里数坐标，直接表示距离）
struct Coordinate {
    double x;  // x轴坐标（公里）
    double y;  // y轴坐标（公里）
    Coordinate(double x = 0, double y = 0) : x(x), y(y) {}
};

// 订单状态枚举
enum class OrderStatus {
    PENDING,    // 待接单
    ACCEPTED,   // 已接单
    COMPLETED,  // 已完成
    CANCELLED   // 已取消
};

// 骑手状态结构体（记录当前状态，用于动态调整优先级权重）
struct RiderStatus {
    Coordinate currentPos;    // 当前位置
    int currentOrderCount;    // 已接订单数（负载）
    double remainingBattery;  // 剩余电量（0-100）
    RiderStatus(Coordinate pos = Coordinate(), int count = 0, double battery = 100)
        : currentPos(pos), currentOrderCount(count), remainingBattery(battery) {
    }
};

// 订单结构体
struct Order {
    string orderId;           // 订单ID
    Coordinate merchantPos;   // 商家位置（校园食堂/社区商铺）
    Coordinate userPos;       // 用户位置（宿舍/居民楼）
    time_t createTime;        // 订单创建时间（时间戳）
    time_t requireArriveTime; // 要求送达时间（时间戳）
    double orderAmount;       // 订单金额（元）
    double avgCookTime;       // 平均出餐时间（分钟）
    OrderStatus status;       // 订单状态

    // 计算骑手当前位置到商家的距离（欧氏距离，单位：km）
    double calcDistanceToMerchant(const Coordinate& riderPos) const {
        double dx = riderPos.x - merchantPos.x;
        double dy = riderPos.y - merchantPos.y;
        return sqrt(dx * dx + dy * dy);
    }

    // 计算订单剩余时间（分钟）
    double getRemainingTime(time_t currentTime) const {
        return difftime(requireArriveTime, currentTime) / 60.0;
    }

    // 动态优先级评分（核心算法：根据骑手状态调整权重）
    double calcPriorityScore(const RiderStatus& riderStatus, time_t currentTime) const {
        if (status != OrderStatus::PENDING) return 1e18;  // 非待接单订单优先级最低

        double remainTime = getRemainingTime(currentTime);
        if (remainTime <= 0) return 0.0;  // 已超时订单优先级最高

        double distance = calcDistanceToMerchant(riderStatus.currentPos);
        double amountFactor = (orderAmount > 0) ? (1.0 / orderAmount) : 0.1;  // 金额越高，因子越小

        // 动态权重：根据骑手负载调整（核心创新点）
        double w_remainTime = 0.4, w_distance = 0.3, w_amount = 0.2, w_cook = 0.1;
        if (riderStatus.currentOrderCount >= 3) {  // 高负载：优先时效性（避免超时）
            w_remainTime = 0.6;
            w_distance = 0.25;
            w_amount = 0.1;
            w_cook = 0.05;
        }
        else if (riderStatus.currentOrderCount == 0) {  // 低负载：优先收益
            w_remainTime = 0.3;
            w_distance = 0.25;
            w_amount = 0.3;
            w_cook = 0.15;
        }

        // 优先级评分：值越小，优先级越高
        return (remainTime * w_remainTime) +
            (distance * w_distance) +
            (amountFactor * w_amount) +
            (avgCookTime * w_cook);
    }
};

// 动态优先级堆（最小堆：评分最小的订单在堆顶，即最优订单）
class DynamicPriorityHeap {
private:
    // 堆节点：存储订单指针和当前优先级评分
    struct HeapNode {
        Order* order;
        double score;
        HeapNode(Order* o = nullptr, double s = 0) : order(o), score(s) {}
        // 最小堆比较规则：score小的优先
        bool operator<(const HeapNode& other) const {
            return score > other.score;
        }
    };

    priority_queue<HeapNode> heap;  // 最小堆
    unordered_map<string, Order*> orderMap;  // 订单ID到订单的映射（快速查询）

public:
    // 新增订单
    void pushOrder(Order* order, const RiderStatus& riderStatus, time_t currentTime) {
        if (!order || order->status != OrderStatus::PENDING) return;
        double score = order->calcPriorityScore(riderStatus, currentTime);
        heap.emplace(order, score);
        orderMap[order->orderId] = order;
    }

    // 获取最优订单（堆顶）
    Order* getTopOrder(const RiderStatus& riderStatus, time_t currentTime) {
        // 移除已取消/已完成/已超时的无效订单
        while (!heap.empty()) {
            HeapNode top = heap.top();
            Order* topOrder = top.order;

            // 重新计算当前评分（订单状态可能已变）
            double currentScore = topOrder->calcPriorityScore(riderStatus, currentTime);

            // 无效订单：弹出并移除映射
            if (topOrder->status != OrderStatus::PENDING || currentScore == 1e18) {
                heap.pop();
                orderMap.erase(topOrder->orderId);
                continue;
            }

            // 评分变化超过阈值（0.1），更新堆节点（简单实现：弹出旧节点，插入新节点）
            if (fabs(currentScore - top.score) > 0.1) {
                heap.pop();
                heap.emplace(topOrder, currentScore);
                continue;
            }

            // 找到有效最优订单
            return topOrder;
        }
        return nullptr;  // 无待接单订单
    }

    // 移除堆顶订单（骑手接单）
    void popTopOrder(const RiderStatus& riderStatus, time_t currentTime) {
        if (heap.empty()) return;  // 先判断堆是否为空
        Order* top = getTopOrder(riderStatus, currentTime);
        if (top) {
            top->status = OrderStatus::ACCEPTED;
            // 由于getTopOrder可能已弹出无效元素，这里无需再次pop（原逻辑重复弹出）
            orderMap.erase(top->orderId);
        }
    }

    // 取消订单
    void cancelOrder(const string& orderId) {
        auto it = orderMap.find(orderId);
        if (it != orderMap.end()) {
            it->second->status = OrderStatus::CANCELLED;
            orderMap.erase(it);
            // 堆中无效节点后续getTop时自动清理
        }
    }

    // 堆大小
    int size() const {
        return orderMap.size();
    }
};

// 工具函数：生成随机订单（模拟校园/社区短途订单）
Order generateRandomOrder(int orderIdx, time_t currentTime) {
    Order order;
    order.orderId = "ORDER_" + to_string(orderIdx);
    order.status = OrderStatus::PENDING;

    // 改为公里坐标，骑手初始位置在(0,0)，商家在0.5~3km范围内随机分布
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> posDist(-3.0, 3.0);  // x,y范围：-3~3公里
    uniform_real_distribution<> amountDist(15.0, 40.0);
    uniform_int_distribution<> cookTimeDist(3, 9);
    uniform_int_distribution<> remainTimeDist(5, 15);

    // 确保商家距离骑手初始位置在0.5~3km之间
    Coordinate merchantPos;
    do {
        merchantPos = Coordinate(posDist(gen), posDist(gen));
    } while (sqrt(merchantPos.x * merchantPos.x + merchantPos.y * merchantPos.y) < 0.5);

    order.merchantPos = merchantPos;
    order.userPos = Coordinate(posDist(gen), posDist(gen));  // 用户位置随机
    order.orderAmount = round(amountDist(gen) * 10) / 10;
    order.avgCookTime = cookTimeDist(gen);
    order.createTime = currentTime;
    order.requireArriveTime = currentTime + remainTimeDist(gen) * 60;

    return order;
}

// 主函数：模拟骑手配送过程，测试算法效果
int main() {
    // 初始化：当前时间、骑手初始状态（校园中心位置）
    time_t currentTime = time(nullptr);
    RiderStatus riderStatus(Coordinate(0, 0), 0, 100);
    DynamicPriorityHeap orderHeap;

    // 生成10个模拟订单（校园饭点峰值订单量）
    cout << "=== 生成模拟短途订单 ===" << endl;
    vector<Order*> orders;  // 改为指针容器
    for (int i = 0; i < 10; ++i) {
        Order* newOrder = new Order(generateRandomOrder(i + 1, currentTime));
        orders.push_back(newOrder);
        orderHeap.pushOrder(newOrder, riderStatus, currentTime);  // 存储稳定指针
        cout << "订单ID: " << newOrder->orderId
            << " | 金额: " << newOrder->orderAmount << "元"
            << " | 剩余时间: " << newOrder->getRemainingTime(currentTime) << "分钟"
            << " | 商家距离: " << newOrder->calcDistanceToMerchant(riderStatus.currentPos) << "km" << endl;
    }

    // 模拟骑手接单-配送-完成的流程（3轮决策）
    cout << "\n=== 骑手订单决策过程 ===" << endl;
    for (int round = 1; round <= 3; ++round) {
        cout << "\n【第" << round << "轮决策】" << endl;
        cout << "骑手当前状态：位置(" << riderStatus.currentPos.x << "," << riderStatus.currentPos.y
            << ") | 已接订单数: " << riderStatus.currentOrderCount << endl;

        // 获取最优订单
        Order* bestOrder = orderHeap.getTopOrder(riderStatus, currentTime);
        if (!bestOrder) {
            cout << "无待接单订单，流程结束" << endl;
            break;
        }

        // 输出最优订单信息
        cout << "推荐最优订单：" << bestOrder->orderId << endl;
        cout << "  - 优先级评分: " << bestOrder->calcPriorityScore(riderStatus, currentTime) << endl;
        cout << "  - 订单金额: " << bestOrder->orderAmount << "元" << endl;
        cout << "  - 剩余时间: " << bestOrder->getRemainingTime(currentTime) << "分钟" << endl;
        cout << "  - 商家距离: " << bestOrder->calcDistanceToMerchant(riderStatus.currentPos) << "km" << endl;

        // 骑手接单（更新骑手状态）
        orderHeap.popTopOrder(riderStatus, currentTime);
        riderStatus.currentOrderCount++;
        cout << "骑手已接单：" << bestOrder->orderId << " | 当前负载: " << riderStatus.currentOrderCount << endl;

        // 模拟配送时间（距离*3分钟/km，短途场景配送时间短）
        double deliveryTime = bestOrder->calcDistanceToMerchant(riderStatus.currentPos) * 3;
        currentTime += deliveryTime * 60;  // 转换为秒
        riderStatus.currentPos = bestOrder->userPos;  // 配送完成后，骑手移动到用户位置
        riderStatus.currentOrderCount--;  // 订单完成，负载减少
        bestOrder->status = OrderStatus::COMPLETED;

        cout << "配送完成：" << bestOrder->orderId << " | 耗时: " << deliveryTime << "分钟" << endl;
    }

    cout << "\n=== 模拟结束 ===" << endl;
    cout << "剩余待接单订单数: " << orderHeap.size() << endl;

    return 0;
}