#include <iostream>
#include <vector>
#include <string>
#include <stack>
using namespace std;

class Solution {
public:    
    bool isNum(string& ch) {
        // 空字符串不是数字
        if (ch.empty()) return false;
        // 第一个字符是 '-' 且长度 >1，后面必须全是数字
        if (ch[0] == '-' && ch.size() > 1) {
            for (int i = 1; i < ch.size(); i++) {
                if (!isdigit(ch[i])) return false;
            }
            return true;
        }
        // 全部字符都是数字
        for (char c : ch) {
            if (!isdigit(c)) return false;
        }
        return true;
    }
    bool isArithmetic(string& ch) {
        return ch == "+" || ch == "-" || ch == "*" || ch == "/";
    }
    int stringToInt(string& s) {
        return stoi(s); // 使用标准库函数，简洁且正确
    }
    int magicMath(int value1, int value2, string& op)
    {
        if (op == "+")
            return value1 + value2;
        else if(op =="-")
            return value2 - value1;
        else if (op == "*")
            return value1 * value2;
        else 
            return value2 / value1;
    }
    int evalRPN(vector<string>& tokens) {
        stack<int> s;
        for (int i = 0; i < tokens.size(); i++)
        {
            if (isNum(tokens[i]))
            {
                s.push(stringToInt(tokens[i]));
            }
            else if (isArithmetic(tokens[i]))
            {
                int value1 = s.top();
                s.pop();
                int value2 = s.top();
                s.pop();
                s.push(magicMath(value1, value2, tokens[i]));
            }
        }
        return s.top();
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