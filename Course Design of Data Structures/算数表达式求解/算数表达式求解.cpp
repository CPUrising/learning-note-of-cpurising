#include <iostream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

// 自定义栈类（替代标准库stack）
template <typename T>
class MyStack {
private:
    T* data;       // 动态数组存储栈元素
    int topIndex;  // 栈顶索引（-1表示空栈）
    int capacity;  // 当前容量
public:
    MyStack(int initCapacity = 10) : capacity(initCapacity), topIndex(-1) {
        data = new T[capacity];
    }
    ~MyStack() {
        delete[] data;  // 释放动态内存
    }
    void push(const T& val) {
        if (topIndex + 1 >= capacity) {
            capacity *= 2;
            T* newData = new T[capacity];
            for (int i = 0; i <= topIndex; ++i) newData[i] = data[i];
            delete[] data;
            data = newData;
        }
        data[++topIndex] = val;
    }
    // 出栈
    void pop() {
        if (!empty()) topIndex--;
    }
    // 获取栈顶元素
    T& top() {
        return data[topIndex];
    }
    const T& top() const {
        return data[topIndex];
    }
    // 判断栈是否为空
    bool empty() const {
        return topIndex == -1;
    }
};

// 检查字符是否合法
bool checkValidChar(char c) {
    return isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/' ||
        c == '(' || c == ')' || c == ' ' || c == '^' || c == '%' ||
        c == '×' || c == '=';
}

// 括号匹配检查（使用自定义栈）
bool checkBrackets(const string& s) {
    MyStack<char> st;
    for (char c : s) {
        if (c == '(') st.push(c);
        else if (c == ')') {
            if (st.empty() || st.top() != '(') return false;
            st.pop();
        }
    }
    return st.empty();
}

// 语法检查
bool checkSyntax(const string& s) {
    if (s.empty()) return false;
    char first = s[0], last = s.back();
    if (first == '*' || first == '/' || first == '%' || first == '^' ||
        last == '+' || last == '-' || last == '*' || last == '/' || last == '%' || last == '^') {
        return false;
    }
    for (size_t i = 1; i < s.size(); ++i) {
        char prev = s[i - 1], curr = s[i];
        bool prevOp = (prev == '+' || prev == '-' || prev == '*' || prev == '/' || prev == '%' || prev == '^');
        bool currOp = (curr == '*' || curr == '/' || curr == '%' || curr == '^');
        if (prevOp && currOp) return false;
        if ((prev == '(' && currOp) || (curr == ')' && prevOp)) return false;
    }
    return true;
}

// 预处理
string preprocess(string expr) {
    string res;
    for (char c : expr) {
        if (c == ' ') continue;
        if (c == '×') res += '*';
        else if (c == '=') break;
        else res += c;
    }
    return res;
}

// 判断是否为数字
bool isNum(const string& ch) {
    if (ch.empty()) return false;
    size_t start = 0;
    if (ch[0] == '-' && ch.size() > 1) start = 1;
    for (size_t i = start; i < ch.size(); ++i) {
        if (!isdigit(ch[i])) return false;
    }
    return true;
}

// 判断是否为双目运算符
bool isArithmetic(const string& ch) {
    return ch == "+" || ch == "-" || ch == "*" || ch == "/" || ch == "^" || ch == "%";
}

// 字符串转整数
int stringToInt(const string& s) {
    return stoi(s);
}

// 运算逻辑
int magicMath(int value1, int value2, const string& op) {
    if (op == "+") return value2 + value1;
    if (op == "-") return value2 - value1;
    if (op == "*") return value2 * value1;
    if (op == "/") {
        if (value1 == 0) { cerr << "错误：除数不能为0" << endl; exit(1); }
        return value2 / value1;
    }
    if (op == "%") {
        if (value1 == 0) { cerr << "错误：取余除数不能为0" << endl; exit(1); }
        return value2 % value1;
    }
    if (op == "^") {
        int res = 1;
        for (int i = 0; i < value1; ++i) res *= value2;
        return res;
    }
    return 0;
}

// 用数组存储运算符及优先级（替代unordered_map）
const string ops[] = { "+", "-", "*", "/", "%", "^" };
const int precs[] = { 1, 1, 2, 2, 2, 3 };
int getPrecedence(const string& op) {
    for (int i = 0; i < 6; ++i) {
        if (ops[i] == op) return precs[i];
    }
    return 0; // 非法运算符（理论上不会走到这里）
}

// 中缀转逆波兰表达式（使用自定义栈）
vector<string> infixToRPN(const string& expr) {
    vector<string> rpn;
    MyStack<string> opsStack;

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isdigit(c)) {
            size_t j = i;
            while (j < expr.size() && isdigit(expr[j])) j++;
            rpn.push_back(expr.substr(i, j - i));
            i = j - 1;
        }
        else if (c == '(') {
            opsStack.push("(");
        }
        else if (c == ')') {
            while (!opsStack.empty() && opsStack.top() != "(") {
                rpn.push_back(opsStack.top());
                opsStack.pop();
            }
            opsStack.pop();
        }
        else if (c == '+' || c == '-') {
            bool isUnary = (i == 0) || (expr[i - 1] == '(') || (isArithmetic(string(1, expr[i - 1])));
            if (isUnary) {
                opsStack.push(c == '+' ? "#" : "$");
            }
            else {
                string op(1, c);
                while (!opsStack.empty() && opsStack.top() != "(" && getPrecedence(opsStack.top()) >= getPrecedence(op)) {
                    rpn.push_back(opsStack.top());
                    opsStack.pop();
                }
                opsStack.push(op);
            }
        }
        else if (c == '*' || c == '/' || c == '%' || c == '^') {
            string op(1, c);
            while (!opsStack.empty() && opsStack.top() != "(" &&
                ((op != "^" && getPrecedence(opsStack.top()) >= getPrecedence(op)) ||
                    (op == "^" && getPrecedence(opsStack.top()) > getPrecedence(op)))) {
                rpn.push_back(opsStack.top());
                opsStack.pop();
            }
            opsStack.push(op);
        }
    }

    while (!opsStack.empty()) {
        rpn.push_back(opsStack.top());
        opsStack.pop();
    }
    return rpn;
}

// 计算逆波兰表达式（使用自定义栈）
int evalRPN(const vector<string>& tokens) {
    MyStack<int> numStack;
    for (const string& token : tokens) {
        if (isNum(token)) {
            numStack.push(stringToInt(token));
        }
        else if (token == "#") {
            int val = numStack.top(); numStack.pop();
            numStack.push(val);
        }
        else if (token == "$") {
            int val = numStack.top(); numStack.pop();
            numStack.push(-val);
        }
        else if (isArithmetic(token)) {
            int value1 = numStack.top(); numStack.pop();
            if (numStack.empty() && (token == "+" || token == "-")) {
                numStack.push(token == "+" ? value1 : -value1);
                continue;
            }
            int value2 = numStack.top(); numStack.pop();
            numStack.push(magicMath(value1, value2, token));
        }
    }
    return numStack.top();
}

// 综合检查表达式合法性
bool checkString(const string& s) {
    for (char c : s) {
        if (!checkValidChar(c)) {
            cerr << "错误：包含非法字符 '" << c << "'" << endl;
            return true;
        }
    }
    if (!checkBrackets(s)) {
        cerr << "错误：括号不匹配" << endl;
        return true;
    }
    string processed = preprocess(s);
    if (!checkSyntax(processed)) {
        cerr << "错误：语法错误" << endl;
        return true;
    }
    return false;
}

// 计算表达式主函数
int calculate(string& s) {
    string processed = preprocess(s);
    vector<string> rpn = infixToRPN(processed);
    return evalRPN(rpn);
}

int main() {
    char cont = 'y';
    while (tolower(cont) == 'y') {
        cout << "输入表达式：";
        string expr;
        getline(cin, expr);

        if (checkString(expr)) {
            cout << "是否继续（y/n）？";
            cin >> cont;
            cin.ignore();
            continue;
        }

        try {
            cout << "计算结果：" << calculate(expr) << endl;
        }
        catch (...) {
            cerr << "计算错误" << endl;
        }

        cout << "是否继续（y/n）？";
        cin >> cont;
        cin.ignore();
    }
    return 0;
}