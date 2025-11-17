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

// 比较函数模板：默认使用 < 运算符
template <typename T>
bool lessThan(const T& a, const T& b) {
    return a < b;
}

// 字符串特化：比较长度
template <>
bool lessThan(const string& a, const string& b) {
    return a.length() < b.length();
}

// Point特化：比较欧氏距离
template <>
bool lessThan(const Point& a, const Point& b) {
    return a.dist() < b.dist();
}

// Point*特化：通过解引用比较
template <typename T>
bool lessThan(const T* a, const T* b) {
    return lessThan(*a, *b);  
}

// 通用最小值查找器：返回最后一个最小值
template <typename T>
T findMin(const T& a, const T& b, const T& c) {
    T minVal = a;
    if (lessThan(minVal, b)) {
        minVal = b;
    }
    if (lessThan(minVal, c)) {
        minVal = c;
    }
    return minVal;
}

// 辅助函数：读取Point
Point readPoint() {
    Point p;
    char comma;
    cin >> p.x >> comma >> p.y;
    return p;
}

// 辅助函数：打印Point
void printPoint(const Point& p) {
    cout << p.x << "," << p.y;
}

int main() {
    // 测试int
    int i1, i2, i3;
    cin >> i1 >> i2 >> i3;
    cout << findMin(i1, i2, i3) << endl;

    // 测试double
    double d1, d2, d3;
    cin >> d1 >> d2 >> d3;
    cout << findMin(d1, d2, d3) << endl;

    // 测试char
    char c1, c2, c3;
    cin >> c1 >> c2 >> c3;
    cout << findMin(c1, c2, c3) << endl;

    // 测试string
    string s1, s2, s3;
    cin >> s1 >> s2 >> s3;
    cout << findMin(s1, s2, s3) << endl;

    // 测试Point
    Point p1 = readPoint();
    Point p2 = readPoint();
    Point p3 = readPoint();
    printPoint(findMin(p1, p2, p3));
    cout << endl;

    // 测试Point*
    Point p4 = readPoint();
    Point p5 = readPoint();
    Point p6 = readPoint();
    Point* pp1 = &p4;
    Point* pp2 = &p5;
    Point* pp3 = &p6;
    printPoint(*findMin(pp1, pp2, pp3));
    cout << endl;

    return 0;
}