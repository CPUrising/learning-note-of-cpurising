#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
struct Counter {
    std::mutex mutex;//lock
    int value;
    Counter() :value(0) {}
    void increment() {
        ++value;
    }
};
struct AtomicCounter {//也可以换成这个
    std::atomic<int> value;
    void increment() {
        ++value;
    }
    void decrement() {
        --value;
    }
    int get() {
        return value.load();
    }
};
int main() {
    Counter counter;
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.push_back(std::thread([&counter]() {
            for (int i = 0; i < 5000; ++i) {
                counter.increment();
            }
            }));
    }
    for (auto& thread : threads) {
        thread.join();
    }
    std::cout << counter.value << std::endl;
    return 0;
}