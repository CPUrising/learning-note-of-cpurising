#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
using namespace std;

class String
{
public:
    // 普通构造函数
    String(const char* pStr = "")
    {
        if (nullptr == pStr)  // 使用nullptr更规范
        {
            pstr = new char[1];
            *pstr = '\0';
        }
        else
        {
            // 分配足够内存（包含结束符）
            pstr = new char[strlen(pStr) + 1];
            strcpy(pstr, pStr);  // 拷贝字符串内容
        }
    }

    // 深拷贝版本拷贝构造函数
    String(const String& s)
    {
        // 为当前对象分配独立内存
        pstr = new char[strlen(s.pstr) + 1];
        // 拷贝内容（而非地址）
        strcpy(pstr, s.pstr);
    }

    // 深拷贝版本赋值运算符重载
    String& operator=(const String& s)
    {
        // 防止自赋值
        if (this != &s)
        {
            // 1. 释放当前对象原有内存
            delete[] pstr;

            // 2. 分配新内存并拷贝内容（深拷贝核心）
            pstr = new char[strlen(s.pstr) + 1];
            strcpy(pstr, s.pstr);
        }
        return *this;
    }

    // 析构函数
    ~String()
    {
        delete[] pstr;  // 释放动态内存
        pstr = nullptr; // 避免野指针
    }

    // 输出运算符重载（友元）
    friend ostream& operator<<(ostream& _cout, const String& s)
    {
        _cout << s.pstr;
        return _cout;
    }

private:
    char* pstr;  // 指向动态分配的字符串内存
};

int main()
{
    String s1("Hello");
    String s2(s1);  // 调用深拷贝构造函数
    String s3("World");
    s3 = s1;        // 调用深拷贝赋值运算符

    cout << "s1: " << s1 << endl;
    cout << "s2: " << s2 << endl;
    cout << "s3: " << s3 << endl;

    // 测试修改一个对象是否影响其他对象（深拷贝保障独立性）
    String s4("Test");
    String s5 = s4;
    // 注意：这里为了演示，实际需要添加修改字符串的成员函数
    // 此处仅作逻辑验证：深拷贝后对象内存独立，修改互不影响

    return 0;
}