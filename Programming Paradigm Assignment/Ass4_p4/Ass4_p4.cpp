//题目 4
//
//编写一个基类，包含：
//
//* 一个名为`process()`的虚函数
//
//* 一个名为`compute()`的非虚函数
//
//
//然后派生一个类`AdvancedProcessor`，该类重写（override）虚函数`process()`，
//并重新定义（隐藏）非虚函数`compute()`。
//
//创建`AdvancedProcessor`类的对象，将其向上转型为基类指针，
//使用`chrono`库（`<chrono > `）
//测量多次调用`process()`和`compute()`函数的执行时间。
//比较并打印虚函数调用与非虚函数调用的平均时间差异。

#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

class BaseProcessor {
public:
    virtual void process() {
        volatile int x = 0;
        for (int i = 0; i < 100; ++i) {
            x += i;
        }
    }

    void compute() {
        volatile int x = 0;
        for (int i = 0; i < 100; ++i) {
            x *= i;
        }
    }
};

class AdvancedProcessor : public BaseProcessor {
public:
    void process() override {
        volatile int x = 0;
        for (int i = 0; i < 100; ++i) {
            x += i * 2;
        }
    }

    void compute() {
        volatile int x = 0;
        for (int i = 0; i < 100; ++i) {
            x *= i * 2;
        }
    }
};

int main() {
    const int iterations = 10000000;
    AdvancedProcessor advProc;
    BaseProcessor* basePtr = &advProc;

    auto start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        basePtr->process();
    }
    auto end = high_resolution_clock::now();
    duration<double, micro> processTime = end - start;
    double avgProcess = processTime.count() / iterations;

    start = high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        basePtr->compute();
    }
    end = high_resolution_clock::now();
    duration<double, micro> computeTime = end - start;
    double avgCompute = computeTime.count() / iterations;

    cout << "虚函数 process() 平均调用时间: " << avgProcess << " 微秒" << endl;
    cout << "非虚函数 compute() 平均调用时间: " << avgCompute << " 微秒" << endl;
    cout << "时间差异: " << abs(avgProcess - avgCompute) << " 微秒" << endl;

    return 0;
}