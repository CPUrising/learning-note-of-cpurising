#include <iostream>
#include <vector>
using namespace std;

// 基类：定义数列生成接口
class num_sequence {
public:
    virtual ~num_sequence() = default; // 虚析构函数
    virtual void generate(int n) = 0;  // 纯虚函数，定义生成接口
};

// 斐波那契数列派生类
class Fibonacci : public num_sequence {
public:
    void generate(int n) override {
        if (n <= 0) {
            cout << "输入必须为正数" << endl;
            return;
        }
        cout << "斐波那契数列前" << n << "项：";
        int a = 0, b = 1;
        for (int i = 0; i < n; ++i) {
            cout << a << " ";
            int c = a + b;
            a = b;
            b = c;
        }
        cout << endl;
    }
};

// 佩尔数列派生类
class Pell : public num_sequence {
public:
    void generate(int n) override {
        if (n <= 0) {
            cout << "输入必须为正数" << endl;
            return;
        }
        cout << "佩尔数列前" << n << "项：";
        int a = 0, b = 1;
        for (int i = 0; i < n; ++i) {
            cout << a << " ";
            int c = 2 * b + a;
            a = b;
            b = c;
        }
        cout << endl;
    }
};

// 卢卡斯数列派生类
class Lucas : public num_sequence {
public:
    void generate(int n) override {
        if (n <= 0) {
            cout << "输入必须为正数" << endl;
            return;
        }
        cout << "卢卡斯数列前" << n << "项：";
        int a = 2, b = 1;
        for (int i = 0; i < n; ++i) {
            cout << a << " ";
            int c = a + b;
            a = b;
            b = c;
        }
        cout << endl;
    }
};

// 三角数列派生类
class Triangular : public num_sequence {
public:
    void generate(int n) override {
        if (n <= 0) {
            cout << "输入必须为正数" << endl;
            return;
        }
        cout << "三角数列前" << n << "项：";
        for (int i = 0; i < n; ++i) {
            cout << i * (i + 1) / 2 << " ";
        }
        cout << endl;
    }
};

// 平方数列派生类
class Square : public num_sequence {
public:
    void generate(int n) override {
        if (n <= 0) {
            cout << "输入必须为正数" << endl;
            return;
        }
        cout << "平方数列前" << n << "项：";
        for (int i = 0; i < n; ++i) {
            cout << i * i << " ";
        }
        cout << endl;
    }
};

// 测试代码
int main() {
    // 使用基类指针指向不同派生类对象，实现多态
    num_sequence* seq = new Fibonacci();
    seq->generate(8);  // 斐波那契数列

    delete seq;
    seq = new Pell();
    seq->generate(8);  // 佩尔数列

    delete seq;
    seq = new Square();
    seq->generate(8);  // 平方数列

    delete seq;
    return 0;
}