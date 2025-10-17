#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 假设Solution类的定义在此处
class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        // 待实现的逆波兰表达式求值逻辑
        return 0; // 临时返回值
    }
};

int main() {
    Solution solution;

    // 测试用例1: 基本运算
    vector<string> test1 = { "2", "1", "+", "3", "*" };
    cout << "测试用例1: " << solution.evalRPN(test1) << endl; // 预期结果: 9

    // 测试用例2: 包含减法和除法
    vector<string> test2 = { "4", "13", "5", "/", "+" };
    cout << "测试用例2: " << solution.evalRPN(test2) << endl; // 预期结果: 6

    // 测试用例3: 包含负数
    vector<string> test3 = { "10", "6", "9", "3", "+", "-11", "*", "/", "*", "17", "+", "5", "+" };
    cout << "测试用例3: " << solution.evalRPN(test3) << endl; // 预期结果: 22

    // 测试用例4: 单元素
    vector<string> test4 = { "5" };
    cout << "测试用例4: " << solution.evalRPN(test4) << endl; // 预期结果: 5

    // 测试用例5: 除法取整
    vector<string> test5 = { "7", "-2", "/" };
    cout << "测试用例5: " << solution.evalRPN(test5) << endl; // 预期结果: -3

    return 0;
}