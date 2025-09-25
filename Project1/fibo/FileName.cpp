#include <iostream>
#include <chrono>  // 用于时间测量
using namespace std;
using namespace chrono;

// 递归实现斐波那契数列
int fibonacciRecursive(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}

// 迭代实现斐波那契数列
int fibonacciIterative(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;

    int a = 0, b = 1, result = 0;
    for (int i = 2; i <= n; ++i) {
        result = a + b;
        a = b;
        b = result;
    }
    return result;
}

// 测量函数执行时间的模板函数
template <typename Func, typename... Args>
long long measureTime(Func func, Args&&... args) {
    // 记录开始时间
    auto start = high_resolution_clock::now();

    // 执行函数
    func(forward<Args>(args)...);

    // 记录结束时间
    auto end = high_resolution_clock::now();

    // 计算时间差（微秒）
    auto duration = duration_cast<microseconds>(end - start);
    return duration.count();
}

int main() {
    int n;
    cout << "请输入斐波那契数列的项数: ";
    cin >> n;

    // 测量递归方法用时
    long long recursiveTime = measureTime(fibonacciRecursive, n);
    // 测量迭代方法用时
    long long iterativeTime = measureTime(fibonacciIterative, n);

    // 输出结果
    cout << "第" << n << "项斐波那契数是: " << fibonacciIterative(n) << endl;
    cout << "递归实现用时: " << recursiveTime << " 微秒" << endl;
    cout << "迭代实现用时: " << iterativeTime << " 微秒" << endl;
   // cout << "迭代方法比递归快约 " << (recursiveTime / (double)iterativeTime) << " 倍" << endl;

    return 0;
}
