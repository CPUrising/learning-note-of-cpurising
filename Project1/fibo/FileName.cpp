#include <iostream>
#include <chrono>  // ����ʱ�����
using namespace std;
using namespace chrono;

// �ݹ�ʵ��쳲���������
int fibonacciRecursive(int n) {
    if (n <= 0) return 0;
    if (n == 1) return 1;
    return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
}

// ����ʵ��쳲���������
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

// ��������ִ��ʱ���ģ�庯��
template <typename Func, typename... Args>
long long measureTime(Func func, Args&&... args) {
    // ��¼��ʼʱ��
    auto start = high_resolution_clock::now();

    // ִ�к���
    func(forward<Args>(args)...);

    // ��¼����ʱ��
    auto end = high_resolution_clock::now();

    // ����ʱ��΢�룩
    auto duration = duration_cast<microseconds>(end - start);
    return duration.count();
}

int main() {
    int n;
    cout << "������쳲��������е�����: ";
    cin >> n;

    // �����ݹ鷽����ʱ
    long long recursiveTime = measureTime(fibonacciRecursive, n);
    // ��������������ʱ
    long long iterativeTime = measureTime(fibonacciIterative, n);

    // ������
    cout << "��" << n << "��쳲���������: " << fibonacciIterative(n) << endl;
    cout << "�ݹ�ʵ����ʱ: " << recursiveTime << " ΢��" << endl;
    cout << "����ʵ����ʱ: " << iterativeTime << " ΢��" << endl;
   // cout << "���������ȵݹ��Լ " << (recursiveTime / (double)iterativeTime) << " ��" << endl;

    return 0;
}
