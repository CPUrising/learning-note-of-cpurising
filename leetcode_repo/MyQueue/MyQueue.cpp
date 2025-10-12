//https://leetcode.cn/problems/implement-queue-using-stacks/
#include <iostream>
#include <cassert>
#include <stdexcept> // 用于异常处理
class MyQueueNode {
public:
    int value;
    MyQueueNode* next;
    MyQueueNode(int val = 0, MyQueueNode* next = nullptr) : value(val), next(next) {};
};
// 假设MyQueue类的实现放在这里
class MyQueue {
private:
    MyQueueNode* head;
    MyQueueNode* rear;

public:
    MyQueue() {
        head = new MyQueueNode();//new return address and pointer accept it
        rear = head;
    }

    void push(int x) {
        MyQueueNode* newnode = new MyQueueNode(x);
        rear = rear->next = newnode;
    }
    bool empty()//queue empty->true
    {
        return head->next == nullptr;
    }
    int pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty queue");
        }
        MyQueueNode* temp = head->next;
        int value = temp->value;
        head->next = temp->next;
        if (temp == rear)//if the last is poped
            rear = head;
        delete temp;
        return value;
    }
    int peek() {
        if (empty()) {
            throw std::runtime_error("Cannot peek from empty queue");
        }
        return head->next->value;
    }
    ~MyQueue() {
        while (!empty()) {
            pop();
        }
        delete head;  // 释放哨兵节点
    }
};
void testQueue() {
    // 测试1：初始化和基本操作
    MyQueue q1;
    assert(q1.empty() == true);

    q1.push(1);
    q1.push(2);
    assert(q1.empty() == false);
    assert(q1.peek() == 1);
    assert(q1.pop() == 1);
    assert(q1.peek() == 2);
    assert(q1.pop() == 2);
    assert(q1.empty() == true);

    // 测试2：连续操作
    MyQueue q2;
    q2.push(10);
    q2.push(20);
    q2.push(30);

    assert(q2.pop() == 10);
    q2.push(40);
    assert(q2.pop() == 20);
    assert(q2.pop() == 30);
    assert(q2.peek() == 40);
    assert(q2.pop() == 40);
    assert(q2.empty() == true);

    // 测试3：异常处理
    bool exceptionThrown = false;
    try {
        q2.pop();  // 从空队列pop
    }
    catch (const std::runtime_error& e) {
        exceptionThrown = true;
    }
    assert(exceptionThrown == true);

    exceptionThrown = false;
    try {
        q2.peek();  // 从空队列peek
    }
    catch (const std::runtime_error& e) {
        exceptionThrown = true;
    }
    assert(exceptionThrown == true);

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    testQueue();
    return 0;
}