#include <iostream>
#include <vector>  // 包含vector头文件

int main() {
    // 创建一个空的vector（存储int类型）
    std::vector<int> numbers;

    //另一种初始化的方式
    /*const int SIZE = 20;
    vector <int> Numbers(SIZE, 18);
    这样所有全初始化18*/


    // 向vector中添加元素
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    // 访问元素（像数组一样使用[]运算符）
    std::cout << "第二个元素: " << numbers[1] << std::endl;

    // 使用at()方法访问（会进行边界检查）
    std::cout << "第三个元素: " << numbers.at(2) << std::endl;

    // 获取vector的大小
    std::cout << "元素数量: " << numbers.size() << std::endl;

    // 遍历vector
    std::cout << "所有元素: ";
    for (int i = 0; i < numbers.size(); ++i) {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;

    // 使用范围for循环遍历（C++11及以上）
    std::cout << "范围for循环遍历: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // 插入元素到指定位置
    numbers.insert(numbers.begin() + 1, 15);  // 在索引1的位置插入15

    // 删除最后一个元素
    numbers.pop_back();

    // 清空vector
    numbers.clear();

    return 0;
}
//std::vector 常用的成员函数：
//size()：返回当前元素数量
//capacity()：返回当前容量（可容纳的元素数量，可能大于 size）
//push_back()：在末尾添加元素
//pop_back()：删除末尾元素
//clear()：清空所有元素
//empty()：判断是否为空
//reserve(n)：预留至少能容纳 n 个元素的空间
//resize(n)：调整容器大小为 n 个元素
//在实际开发中，std::vector 通常是动态数组的首选，它既保留了数组的高效访问特性，又提供了动态管理的便利性。