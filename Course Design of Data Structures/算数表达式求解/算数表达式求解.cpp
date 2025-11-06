//2458620 古贺 终于后悔反省了
//https://www.doubao.com/chat/27872515546420482
//借鉴了leetcode的逆波兰
//但是栈以及队列还要自己实现
#include <iostream>
#include<stack>
#include<string>
#include <vector>
using namespace std;


bool checkString(string& s);//包含所有的check?
bool checkValid(string& s);//检查是否有不合法的字符，针对单个字符
bool checkBrackets(string& s);//括号检验
bool checkSyntax(string& s);//语法检查(e.g., 2++3, 3*, (+))
bool checkDivisionZero(string& s);//除数或模是否为0,在中途计算时检查就可以，不一定需要专门写
void a();//消除小数，以进行取余运算
void b();//处理单目运算符
string preprocess(string expr);//统一为+-*/以及^
bool isNum(string& ch);
bool isArithmetic(string& ch);
int stringToInt(string& s);
int magicMath(int value1, int value2, string& op);
int calculate(string& s);//输入没有问题
int main()
{
    char cont = 'y';
    while (tolower(cont) == 'y') {
        cout << "输入表达式：";
        string expr;
        getline(cin, expr);
        if (checkString(expr))
            continue;
        cout << calculate(expr) << endl;
        cout << "是否继续（y，n）？n\n";
    }
    return 0;
}
bool checkBrackets(string& s)
{
    if (s.size() % 2)
        return false;
    stack<char>st;
    for (char c : s)
    {
        if (c == '(' || c == '[' || c == '{')
            st.push(c);
        else if (c == ')' || c == ']' || c == '}')
            if (st.empty())
                return false;
            else if (c == ')' && st.top() == '('
                || c == ']' && st.top() == '['
                || c == '}' && st.top() == '{')
                st.pop();
            else return false;
    }
    return st.empty();
}
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
    else if (op == "-")
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
int calculate(string& s)
{
    stack<char> expression;
    for (char c : s)
    {
        if (c == ' ')
            continue;
        else
        {
            expression.push(c);
        }
    }

}
