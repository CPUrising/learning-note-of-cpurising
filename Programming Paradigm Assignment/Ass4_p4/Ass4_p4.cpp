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