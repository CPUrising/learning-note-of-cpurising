#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>

//new其实是运算符...
class DynamicString {
private:
    char* str;  // 指向动态分配的字符串

public:
    // 构造函数：申请动态内存
    DynamicString(const char* initialStr = "") {
        // 为字符串分配内存（加1是为了存储终止符'\0'）
        str = new char[std::strlen(initialStr) + 1];

        // 复制字符串内容
        std::strcpy(str, initialStr);

        std::cout << "构造函数：分配了 " << (std::strlen(initialStr) + 1) << " 字节内存" << std::endl;
    }

    // 析构函数：释放动态内存
    ~DynamicString() {
        if (str != nullptr) {
            delete[] str;  // 释放动态分配的数组
            str = nullptr;  // 避免野指针
            std::cout << "析构函数：释放了内存" << std::endl;
        }
    }

    // 辅助函数：获取字符串
    const char* getString() const {
        return str;
    }

    // 辅助函数：修改字符串
    void setString(const char* newStr) {
        // 先释放旧内存
        delete[] str;

        // 分配新内存并复制内容
        str = new char[std::strlen(newStr) + 1];
        std::strcpy(str, newStr);
    }
};

int main() {
    // 创建对象，触发构造函数
    DynamicString str1("Hello, World!");
    std::cout << "str1: " << str1.getString() << std::endl;

    // 修改字符串内容
    str1.setString("Dynamic memory management");
    std::cout << "修改后 str1: " << str1.getString() << std::endl;

    // 创建另一个对象
    DynamicString str2;
    str2.setString("Another string");
    std::cout << "str2: " << str2.getString() << std::endl;

    // 程序结束时，对象会被销毁，触发析构函数
    return 0;
}