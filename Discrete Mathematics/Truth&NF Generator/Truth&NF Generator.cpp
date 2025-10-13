#include <iostream>
#include <string>
#include <map>
#include <stack>
using namespace std;

typedef map<char, int> Map_ci;
typedef map<int, char> Map_ic;
typedef map<int, int> Map_ii;

Map_ci priority;

// 函数声明
Map_ic getProposition(string formula);
int findProposition(Map_ic, char p);
int pow2(int n);
Map_ii toBinary(int n_proposition, int index);
int calculate(string formula, Map_ic pSet, Map_ii value);
void check(stack<int>& value, stack<char>& opter);
bool checkParentheses(const string& formula);       // 检查括号匹配
bool checkOperatorPosition(const string& formula);  // 检查运算符位置
bool isEmptyFormula(const string& formula);         // 检查公式是否为空
// 检测并处理输入中的#符号（#是系统终止符，不允许用户输入）
bool checkIllegalCharacters(const string& formula);
bool checkConsecutiveLetters(const string& formula);
int main()
{
    priority['('] = 6;
    priority[')'] = 6;
    priority['!'] = 5;
    priority['&'] = 4;
    priority['|'] = 3;
    priority['^'] = 2;
    priority['~'] = 1;
    priority['#'] = 0;
    // 初始化运算符优先级

    cout << "***************************************\n";
    cout << "**                                   **\n";
    cout << "**         欢迎使用逻辑运算器        **\n";
    cout << "**   (计算真值表,合取范式,析取范式)  **\n";
    cout << "**                                   **\n";
    cout << "**              !表示非            **\n";
    cout << "**              &表示合取           **\n";
    cout << "**              |表示析取           **\n";
    cout << "**             ^表示蕴含            **\n";
    cout << "**             ~表示等价            **\n";
    cout << "**                                   **\n";
    cout << "***************************************\n\n";
    char continueFlag = 'y';
    do {
        cout << "请输入合法的命题公式: " << endl;
        string formula;
        cin >> formula;

        if (!checkIllegalCharacters(formula)) {
            continue; // 存在非法符号，直接要求重新输入
        }
        if (!checkConsecutiveLetters(formula)) {
            continue; // 连续的字母，杀
        }
        // 语法检查
        if (isEmptyFormula(formula)) {
            cout << "错误: 公式不能为空!" << endl;
            continue;
        }
        if (!checkParentheses(formula)) {
            cout << "错误: 括号不匹配!" << endl;
            continue;
        }
        if (!checkOperatorPosition(formula)) {
            cout << "错误: 运算符位置不正确!" << endl;
            continue;
        }
        // 检查是否为单个命题变元或是否缺少运算符
        bool hasOperator = false;
        for (char c : formula) {
            if (priority.count(c) && c != '(' && c != ')') {  // 排除括号，只检查运算符
                hasOperator = true;
                break;
            }
        }
        // 单个命题变元如"a"是合法的，多个字符且无运算符如"abc"是不合法的
        if (!hasOperator && formula.size() > 1) {
            cout << "错误: 公式缺少运算符! 若为单个命题变元请输入单个字符" << endl;
            continue;
        }

        Map_ic proposition_set = getProposition(formula);
        cout << "公式中的命题变元数量为: " << proposition_set.size() << endl << "真值表如下: " << endl;
        for (unsigned int i = 0; i < proposition_set.size(); i++)
        {
            cout << proposition_set[i] << "\t";
        }
        cout << formula << endl;

        int* m;
        m = (int*)malloc(sizeof(int) * pow2(proposition_set.size()));   //存储各组赋值下命题公式的真值(1或0)

        for (int i = 0; i < pow2(proposition_set.size()); i++)
        {
            Map_ii bina_set = toBinary(proposition_set.size(), i);
            for (unsigned int j = 0; j < bina_set.size(); j++)
            {
                cout << bina_set[j] << "\t";
            }
            int result = calculate(formula, proposition_set, bina_set);
            *(m + i) = result;
            cout << result << endl;
        }

        int n_m = 0, n_M = 0;
        cout << "该命题公式的析取范式为: " << endl;
        for (int i = 0; i < pow2(proposition_set.size()); i++)
        {
            if (*(m + i) == 1)
            {
                if (n_m == 0)
                {
                    cout << "m<" << i << ">";
                }
                else
                {
                    cout << " \\/ m<" << i << "> ";
                }
                n_m++;
            }
        }
        if (n_m == 0)
        {
            cout << "0";
        }
        cout << endl;

        cout << "该命题公式的合取范式为: " << endl;
        for (int i = 0; i < pow2(proposition_set.size()); i++)
        {
            if (*(m + i) == 0)
            {
                if (n_M == 0)
                {
                    cout << "M<" << i << ">";
                }
                else
                {
                    cout << " /\\ M<" << i << "> ";
                }
                n_M++;
            }
        }
        if (n_M == 0)
        {
            cout << "0";
        }
        cout << endl;

        free(m);  // 释放动态分配的内存

        cout << "\n是否继续运算?（y/n）";
        cin >> continueFlag;
    } while (continueFlag == 'y' || continueFlag == 'Y');

    cout << "感谢使用，程序已退出！" << endl;
    return 0;
}
bool checkConsecutiveLetters(const string& formula) {
    for (size_t i = 1; i < formula.length(); ++i) {
        // 判断当前字符和前一个字符是否都是字母
        bool prevIsLetter = ((formula[i - 1] >= 'a' && formula[i - 1] <= 'z') ||
            (formula[i - 1] >= 'A' && formula[i - 1] <= 'Z'));
        bool currIsLetter = ((formula[i] >= 'a' && formula[i] <= 'z') ||
            (formula[i] >= 'A' && formula[i] <= 'Z'));
        if (prevIsLetter && currIsLetter) {
            cout << "错误：存在连续字母！请在字母间添加运算符后重新输入。" << endl;
            return false;
        }
    }
    return true;
}
// 检查是否为空公式
bool isEmptyFormula(const string& formula) {
    return formula.empty();
}
bool checkIllegalCharacters(const string& formula) {
    // 遍历公式中的每个字符
    for (char c : formula) {
        // 合法字符判断：3类合法情况
        bool isLegal = false;
        // 1. 命题变元（a-z 或 A-Z）
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            isLegal = true;
        }
        // 2. 允许的运算符（! & | ^ ~）
        else if (c == '!' || c == '&' || c == '|' || c == '^' || c == '~') {
            isLegal = true;
        }
        // 3. 允许的括号（()）
        else if (c == '(' || c == ')') {
            isLegal = true;
        }

        // 若存在非法字符，立即提示并返回false
        if (!isLegal) {
            cout << "错误：检测到非法符号！请重新输入" << endl;
            return false;
        }
    }
    // 所有字符均合法
    return true;
}
// 检查括号匹配
bool checkParentheses(const string& formula) {
    stack<char> st;
    for (char c : formula) {
        if (c == '(') {
            st.push(c);
        }
        else if (c == ')') {
            if (st.empty() || st.top() != '(') {
                return false; // 右括号过多或不匹配
            }
            st.pop();
        }
    }
    return st.empty(); // 确保所有左括号都有匹配
}

// 检查运算符位置合法性
bool checkOperatorPosition(const string& formula) {
    // 首字符不能是双目运算符（除了!和(）
    if (priority.count(formula[0]) && formula[0] != '!' && formula[0] != '(') {
        return false;
    }

    // 尾字符不能是运算符（除了)）
    char last = formula.back();
    if (priority.count(last) && last != ')') {
        return false;
    }

    // 检查运算符之间、运算符与括号之间的位置关系
    for (size_t i = 1; i < formula.size(); ++i) {
        char prev = formula[i - 1];
        char curr = formula[i];

        // 左括号后不能是双目运算符
        if (prev == '(' && priority.count(curr) && curr != '!' && curr != '(') {
            return false;
        }

        // 右括号后不能是命题变元或左括号
        if (prev == ')' && ((curr >= 'a' && curr <= 'z') ||
            (curr >= 'A' && curr <= 'Z') || curr == '(')) {
            return false;
        }

        // 双目运算符前后不能都是运算符
        if (priority.count(prev) && prev != '!' && prev != '(' && prev != ')') {
            if (priority.count(curr) && curr != '!' && curr != '(') {
                return false;
            }
        }
    }
    return true;
}

int findProposition(Map_ic pSet, char p) {
    Map_ic::iterator it = pSet.begin();
    while (it != pSet.end())
    {
        if (it->second == p)
        {
            return it->first;
        }
        it++;
    }
    return -1;
}

Map_ic getProposition(string formula) {
    Map_ic proposition;
    int n_proposition = 0;
    for (unsigned int i = 0; i < formula.length(); i++)
    {
        char c = formula[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            int r = findProposition(proposition, c);
            if (r == -1)
            {
                proposition[n_proposition] = c;
                n_proposition++;
            }
        }
        else if (!priority.count(c))
        {
            cout << "错误: 存在未定义字符 '" << c << "'" << endl;
            exit(2);
        }
    }
    return proposition;
}

Map_ii toBinary(int n_proposition, int index) {
    Map_ii result;
    for (int i = 0; i < n_proposition; i++)
    {
        int r = index % 2;
        result[n_proposition - 1 - i] = r;
        index = index / 2;
    }
    return result;
}

int pow2(int n) {
    if (n == 0)
        return 1;
    else
        return 2 * pow2(n - 1);
}

int calculate(string formula, Map_ic pSet, Map_ii value) {
    stack<char> opter;
    stack<int> pvalue;
    opter.push('#');
    formula = formula + "#";
    for (unsigned int i = 0; i < formula.length(); i++)
    {
        char c = formula[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            pvalue.push(value[findProposition(pSet, c)]);
        }
        else
        {
            char tmp = opter.top();
            if (priority[tmp] > priority[c])
            {
                while (priority[tmp] > priority[c] && tmp != '(')
                {
                    check(pvalue, opter);
                    tmp = opter.top();
                    if (tmp == '#' && c == '#')
                    {
                        return pvalue.top();
                    }
                }
                opter.push(c);
            }
            else
                opter.push(c);
        }
    }
    return -1;
}

void check(stack<int>& value, stack<char>& opter) {
    int p, q, result;
    char opt = opter.top();

    switch (opt)
    {
    case '&':
        p = value.top();
        value.pop();
        q = value.top();
        value.pop();
        result = p && q;
        value.push(result);
        opter.pop();
        break;

    case '|':
        p = value.top();
        value.pop();
        q = value.top();
        value.pop();
        result = p || q;
        value.push(result);
        opter.pop();
        break;

    case '!':
        p = value.top();
        value.pop();
        result = !p;
        value.push(result);
        opter.pop();
        break;

    case '^':
        q = value.top();
        value.pop();
        p = value.top();
        value.pop();
        result = !p || q;
        value.push(result);
        opter.pop();
        break;

    case '~':
        p = value.top();
        value.pop();
        q = value.top();
        value.pop();
        result = (!p || q) && (p || !q);
        value.push(result);
        opter.pop();
        break;

    case '#':
        break;

    case '(':
        break;

    case ')':
        opter.pop();
        while (opter.top() != '(')
        {
            check(value, opter);
        }
        if (opter.top() == '(')
        {
            opter.pop();
        }
        break;

    default:
        break;
    }
}