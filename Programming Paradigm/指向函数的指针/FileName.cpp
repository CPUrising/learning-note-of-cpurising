#include <iostream>
#include <vector>
using namespace std;

class num_sequence {
public:
    // 定义指向成员函数的指针类型（返回void，参数int）
    typedef void (num_sequence::* PtrType)(int);

    // 构造函数：默认指向fibonacci
    num_sequence() : pmf(&num_sequence::fibonacci) {}

    // 设置要使用的数列（通过成员函数指针）
    void set_sequence(PtrType ptr) {
        if (ptr != nullptr) {
            pmf = ptr;
        }
    }

    // 生成并打印数列的前n项（通过指针调用成员函数）
    void generate(int n) {
        if (n <= 0) {
            cout << "请输入正整数" << endl;
            return;
        }
        // 调用pmf指向的成员函数（必须通过对象调用：this->*pmf）
        (this->*pmf)(n);
    }

    // 各数列的具体实现（生成前n项并打印）
    void fibonacci(int n) {
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

    void pell(int n) {
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

    void lucas(int n) {
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

    void triangular(int n) {
        cout << "三角形数数列前" << n << "项：";
        for (int i = 0; i < n; ++i) {
            cout << i * (i + 1) / 2 << " ";
        }
        cout << endl;
    }

    void square(int n) {
        cout << "平方数数列前" << n << "项：";
        for (int i = 0; i < n; ++i) {
            cout << i * i << " ";
        }
        cout << endl;
    }

private:
    PtrType pmf;  // 成员函数指针，指向当前选中的数列生成函数
};

// 测试代码
int main() {
    num_sequence seq;

    // 生成斐波那契数列（默认）
    seq.generate(8);

    // 切换为佩尔数列
    seq.set_sequence(&num_sequence::pell);
    seq.generate(8);

    // 切换为平方数数列
    seq.set_sequence(&num_sequence::square);
    seq.generate(8);

    return 0;
}