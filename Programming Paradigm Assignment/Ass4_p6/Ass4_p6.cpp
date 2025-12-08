//题目 6：支持自定义比较的通用最小值查找器
//
//### 描述
//
//设计一个函数模板`findMin`，用于查找三个不同类型值中的最小值。若多个元素具有相同的最小值，函数应返回最后一个达到该最小值的元素。
//
//需支持以下数据类型：
//
//* 整数（int）
//
//* 双精度浮点数（double）
//
//* 字符（char）
//
//* 字符串（string，长度较短的字符串视为更小）
//
//* 点结构体（Point，包含整数类型的 x 和 y 坐标，欧氏距离较小的视为更小）
//
//* 点指针（Point\ * ，通过解引用指针的值进行比较）
//
//
//### 示例输入
//
//1 2 3
//
//1.3 2.4 0.6
//
//a b c
//
//hello hi he
//
//1, 2 3, 4 0, 1
//
//1, 2 0, 1 3, 4
//
//### 预期输出
//
//1
//
//0.6
//
//a
//
//he
//
//0, 1
//
//0, 1
//
//### 提示
//
//* 定义结构体：`struct Point{ int x, y; double dist() const; }; `
//
//* 实现`operator < `或重载模板以支持自定义比较。



#include <iostream>
#include <string>
#include <cmath>

using namespace std;

struct Point {
    int x, y;
    double dist() const {
        return sqrt(x * x + y * y);
    }
};

// 比较模板，默认使用 < 运算符
template <typename T>
bool lessThan(const T& a, const T& b) {
    return a < b;
}

// 字符串特化比较（按长度）
template <>
bool lessThan(const string& a, const string& b) {
    return a.length() < b.length();
}

// Point特化比较（按欧氏距离）
template <>
bool lessThan(const Point& a, const Point& b) {
    return a.dist() < b.dist();
}

// 指针特化比较（通过解引用比较）
template <typename T>
bool lessThan(const T* a, const T* b) {
    return lessThan(*a, *b);
}

// 查找三个值中的最小值，若有多个最小值则返回最后一个
template <typename T>
T findMin(const T& a, const T& b, const T& c) {
    T minVal = a;
    if (lessThan(b, minVal)) {  // 若b更小，则更新
        minVal = b;
    }
    if (lessThan(c, minVal)) {  // 若c更小，则更新
        minVal = c;
    }
    return minVal;
}

// 读取Point对象
Point readPoint() {
    Point p;
    char comma;
    cin >> p.x >> comma >> p.y;
    return p;
}

// 打印Point对象
void printPoint(const Point& p) {
    cout << p.x << "," << p.y;
}

int main() {
    // 处理int类型输入
    int i1, i2, i3;
    cout << "请输入三个整数: ";
    cin >> i1 >> i2 >> i3;
    cout << "最小整数是: " << findMin(i1, i2, i3) << endl << endl;

    // 处理double类型输入
    double d1, d2, d3;
    cout << "请输入三个双精度浮点数: ";
    cin >> d1 >> d2 >> d3;
    cout << "最小浮点数是: " << findMin(d1, d2, d3) << endl << endl;

    // 处理char类型输入
    char c1, c2, c3;
    cout << "请输入三个字符: ";
    cin >> c1 >> c2 >> c3;
    cout << "最小字符是: " << findMin(c1, c2, c3) << endl << endl;

    // 处理string类型输入
    string s1, s2, s3;
    cout << "请输入三个字符串: ";
    cin >> s1 >> s2 >> s3;
    cout << "最短字符串是: " << findMin(s1, s2, s3) << endl << endl;

    // 处理Point类型输入
    cout << "请输入三个点(格式x,y): ";
    Point p1 = readPoint();
    Point p2 = readPoint();
    Point p3 = readPoint();
    cout << "距离原点最近的点是: ";
    printPoint(findMin(p1, p2, p3));
    cout << endl << endl;

    // 处理Point*类型输入
    cout << "请输入三个点(格式x,y): ";
    Point p4 = readPoint();
    Point p5 = readPoint();
    Point p6 = readPoint();
    Point* pp1 = &p4;
    Point* pp2 = &p5;
    Point* pp3 = &p6;
    cout << "距离原点最近的点是: ";
    printPoint(*findMin(pp1, pp2, pp3));
    cout << endl;

    return 0;
}