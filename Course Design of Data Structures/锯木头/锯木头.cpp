#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// 自定义最小堆（优化扩容逻辑，适配1e4数据量）
class MinHeap {
private:
    vector<int> heap;
    int size;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[parent] <= heap[idx]) break;
            swap(heap[parent], heap[idx]);
            idx = parent;
        }
    }

    void heapifyDown(int idx) {
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int minIdx = idx;

            if (left < size && heap[left] < heap[minIdx]) minIdx = left;
            if (right < size && heap[right] < heap[minIdx]) minIdx = right;
            if (minIdx == idx) break;

            swap(heap[idx], heap[minIdx]);
            idx = minIdx;
        }
    }

public:
    // 预分配容量，减少扩容开销（适配1e4数据）
    MinHeap(int capacity = 10000) {
        heap.reserve(capacity);
        size = 0;
    }

    void push(int val) {
        heap.push_back(val);
        heapifyUp(size++);
    }

    int pop() {
        if (empty()) return -1; // 理论上不会触发（已做前置校验）
        int top = heap[0];
        heap[0] = heap[--size];
        heap.pop_back();
        heapifyDown(0);
        return top;
    }

    bool empty() const {
        return size == 0;
    }

    int getSize() const {
        return size;
    }
};

// 输入合法性校验
bool validateInput(int N, const vector<int>& lengths) {
    // 校验N的范围（1≤N≤1e4）
    if (N < 1 || N > 10000) {
        cerr << "错误：N必须是1到10000之间的正整数" << endl;
        return false;
    }
    // 校验木头长度（正整数，且总和不溢出int）
    long long total = 0;
    for (int len : lengths) {
        if (len <= 0) {
            cerr << "错误：木头长度必须是正整数" << endl;
            return false;
        }
        total += len;
        // 避免总长度溢出（int最大约2e9，这里限制总长度≤1e18，满足实际需求）
        if (total > LLONG_MAX / 2) {
            cerr << "错误：木头总长度过大" << endl;
            return false;
        }
    }
    return true;
}

// 大数据测试用例生成（可选调用）
void generateBigTestData(int N, vector<int>& lengths) {
    srand(time(0)); // 随机种子
    lengths.clear();
    for (int i = 0; i < N; ++i) {
        // 生成1-1000之间的随机正整数
        lengths.push_back(rand() % 1000 + 1);
    }
    cout << "生成" << N << "个随机木头长度（1-1000）：" << endl;
    for (int i = 0; i < N && i < 10; ++i) { // 只打印前10个
        cout << lengths[i] << " ";
    }
    if (N > 10) cout << "...";
    cout << endl;
}

int main() {
    int N;
    vector<int> lengths;
    char useTestData;

    cout << "是否使用大数据测试用例？（y/n，n则手动输入）：";
    cin >> useTestData;
    cin.ignore(); // 忽略缓冲区换行符

    if (tolower(useTestData) == 'y') {
        // 生成大数据测试用例（默认1e4个，可修改）
        N = 10000;
        generateBigTestData(N, lengths);
    }
    else {
        // 手动输入
        cout << "请输入N（1≤N≤10000）：";
        cin >> N;
        cout << "请输入" << N << "个正整数（木头长度）：";
        lengths.resize(N);
        for (int i = 0; i < N; ++i) {
            cin >> lengths[i];
        }
    }

    // 输入校验
    if (!validateInput(N, lengths)) {
        return 1;
    }

    // 构建最小堆并计算最小花费
    MinHeap heap(N); // 预分配容量
    for (int len : lengths) {
        heap.push(len);
    }

    long long totalCost = 0; // 用long long避免溢出（N=1e4时总花费可能达1e7级别）
    while (heap.getSize() > 1) {
        int a = heap.pop();
        int b = heap.pop();
        int cost = a + b;
        totalCost += cost;
        heap.push(cost);
    }

    cout << "最小锯木花费：" << totalCost << endl;
    return 0;
}