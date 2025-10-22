#include <iostream>
#include <vector>
#include <string>
#include <stack>
using namespace std;

class Solution {
public:    
    bool isNum(string& ch) {
        // ���ַ�����������
        if (ch.empty()) return false;
        // ��һ���ַ��� '-' �ҳ��� >1���������ȫ������
        if (ch[0] == '-' && ch.size() > 1) {
            for (int i = 1; i < ch.size(); i++) {
                if (!isdigit(ch[i])) return false;
            }
            return true;
        }
        // ȫ���ַ���������
        for (char c : ch) {
            if (!isdigit(c)) return false;
        }
        return true;
    }
    bool isArithmetic(string& ch) {
        return ch == "+" || ch == "-" || ch == "*" || ch == "/";
    }
    int stringToInt(string& s) {
        return stoi(s); // ʹ�ñ�׼�⺯�����������ȷ
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

    // ��������1: ��������
    vector<string> test1 = { "2", "1", "+", "3", "*" };
    cout << "��������1: " << solution.evalRPN(test1) << endl; // Ԥ�ڽ��: 9

    // ��������2: ���������ͳ���
    vector<string> test2 = { "4", "13", "5", "/", "+" };
    cout << "��������2: " << solution.evalRPN(test2) << endl; // Ԥ�ڽ��: 6

    // ��������3: ��������
    vector<string> test3 = { "10", "6", "9", "3", "+", "-11", "*", "/", "*", "17", "+", "5", "+" };
    cout << "��������3: " << solution.evalRPN(test3) << endl; // Ԥ�ڽ��: 22

    // ��������4: ��Ԫ��
    vector<string> test4 = { "5" };
    cout << "��������4: " << solution.evalRPN(test4) << endl; // Ԥ�ڽ��: 5

    // ��������5: ����ȡ��
    vector<string> test5 = { "7", "-2", "/" };
    cout << "��������5: " << solution.evalRPN(test5) << endl; // Ԥ�ڽ��: -3

    return 0;
}