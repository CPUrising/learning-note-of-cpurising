#include <iostream>
#include <vector>
#include <limits>
using namespace std;

// 自定义队列：基于单链表实现（适配int类型，支持核心操作）
template <typename T>
class MyQueue {
private:
    // 链表节点结构体
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(val), next(nullptr) {}
    };

    Node* frontNode; // 队头指针
    Node* rearNode;  // 队尾指针
    int size;        // 队列元素个数

public:
    // 构造函数：初始化空队列
    MyQueue() : frontNode(nullptr), rearNode(nullptr), size(0) {}

    // 析构函数：释放所有节点内存
    ~MyQueue() {
        while (!empty()) {
            pop();
        }
    }

    // 入队：队尾添加元素
    void push(const T& val) {
        Node* newNode = new Node(val);
        if (empty()) {
            frontNode = newNode;
            rearNode = newNode;
        }
        else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        size++;
    }

    // 出队：删除队头元素（需先判空）
    void pop() {
        if (empty()) return;
        Node* temp = frontNode;
        frontNode = frontNode->next;
        if (frontNode == nullptr) { // 出队后队列空
            rearNode = nullptr;
        }
        delete temp;
        size--;
    }

    // 获取队头元素（需先判空）
    T front() const {
        return frontNode->data;
    }

    // 判断队列是否为空
    bool empty() const {
        return size == 0;
    }

    // 获取队列大小
    int getSize() const {
        return size;
    }
};

// 输入合法性校验
bool validateInput(int N, const vector<int>& customers) {
    if (N < 1 || N > 1000) {
        cerr << "错误：顾客总数N必须是1到1000之间的正整数" << endl;
        return false;
    }
    for (int id : customers) {
        if (id <= 0) {
            cerr << "错误：顾客编号必须是正整数（当前无效编号：" << id << "）" << endl;
            return false;
        }
    }
    return true;
}

// 处理单组业务数据
void processSingleCase() {
    int N = 0;
    vector<int> customers;
    bool inputValid = false;

    while (!inputValid) {
        cout << "请输入顾客总数N和N个顾客编号（空格分隔）：";
        // 先读取N
        if (!(cin >> N)) {
            cerr << "错误：顾客总数N必须是整数" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // 读取N个顾客ID
        customers.resize(N);
        bool idsValid = true;
        for (int i = 0; i < N; ++i) {
            if (!(cin >> customers[i])) {
                cerr << "错误：顾客编号必须是整数" << endl;
                idsValid = false;
                break;
            }
        }

        if (!idsValid) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // 验证输入合法性
        if (validateInput(N, customers)) {
            inputValid = true;
        }
        // 若验证失败，会自动重新循环
    }

    // 使用自定义队列存储两个窗口的顾客
    MyQueue<int> windowA; // 奇数编号（A窗口：处理速度2倍）
    MyQueue<int> windowB; // 偶数编号（B窗口：处理速度1倍）
    for (int id : customers) {
        if (id % 2 != 0) {
            windowA.push(id);
        }
        else {
            windowB.push(id);
        }
    }

    // 按规则调度处理
    vector<int> result;
    while (!windowA.empty() || !windowB.empty()) {
        // A窗口处理2人（若有）
        for (int i = 0; i < 2 && !windowA.empty(); ++i) {
            result.push_back(windowA.front());
            windowA.pop();
        }
        // B窗口处理1人（若有）
        if (!windowB.empty()) {
            result.push_back(windowB.front());
            windowB.pop();
        }
    }

    // 输出结果
    cout << "业务完成顺序：";
    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) cout << " ";
        cout << result[i];
    }
    cout << endl << "------------------------" << endl;
}

void inputChar(char& len)//cin and check
{
    while (1)
    {
        cin >> len;
        if (!cin.good() || len != 'y' && len != 'n')
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please re-enter y or n:\n";
        }
        else
            break;
    }
}
int main() {
    char cont = 'y';
    cout << "===== 银行业务处理系统 =====" << endl;

    while (tolower(cont) == 'y') {
        processSingleCase();

        cout << "是否继续处理下一组数据？（y/n）：";
		inputChar(cont);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "程序结束，感谢使用！" << endl;
    return 0;
}