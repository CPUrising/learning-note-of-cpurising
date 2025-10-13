#include <iostream>
#include <string>
#include <map>
#include <stack>
using namespace std;

typedef map<char, int> Map_ci;
typedef map<int, char> Map_ic;
typedef map<int, int> Map_ii;

Map_ci priority;

// ��������
Map_ic getProposition(string formula);
int findProposition(Map_ic, char p);
int pow2(int n);
Map_ii toBinary(int n_proposition, int index);
int calculate(string formula, Map_ic pSet, Map_ii value);
void check(stack<int>& value, stack<char>& opter);
bool checkParentheses(const string& formula);       // �������ƥ��
bool checkOperatorPosition(const string& formula);  // ��������λ��
bool isEmptyFormula(const string& formula);         // ��鹫ʽ�Ƿ�Ϊ��
// ��Ⲣ���������е�#���ţ�#��ϵͳ��ֹ�����������û����룩
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
    // ��ʼ����������ȼ�

    cout << "***************************************\n";
    cout << "**                                   **\n";
    cout << "**         ��ӭʹ���߼�������        **\n";
    cout << "**   (������ֵ��,��ȡ��ʽ,��ȡ��ʽ)  **\n";
    cout << "**                                   **\n";
    cout << "**              !��ʾ��            **\n";
    cout << "**              &��ʾ��ȡ           **\n";
    cout << "**              |��ʾ��ȡ           **\n";
    cout << "**             ^��ʾ�̺�            **\n";
    cout << "**             ~��ʾ�ȼ�            **\n";
    cout << "**                                   **\n";
    cout << "***************************************\n\n";
    char continueFlag = 'y';
    do {
        cout << "������Ϸ������⹫ʽ: " << endl;
        string formula;
        cin >> formula;

        if (!checkIllegalCharacters(formula)) {
            continue; // ���ڷǷ����ţ�ֱ��Ҫ����������
        }
        if (!checkConsecutiveLetters(formula)) {
            continue; // ��������ĸ��ɱ
        }
        // �﷨���
        if (isEmptyFormula(formula)) {
            cout << "����: ��ʽ����Ϊ��!" << endl;
            continue;
        }
        if (!checkParentheses(formula)) {
            cout << "����: ���Ų�ƥ��!" << endl;
            continue;
        }
        if (!checkOperatorPosition(formula)) {
            cout << "����: �����λ�ò���ȷ!" << endl;
            continue;
        }
        // ����Ƿ�Ϊ���������Ԫ���Ƿ�ȱ�������
        bool hasOperator = false;
        for (char c : formula) {
            if (priority.count(c) && c != '(' && c != ')') {  // �ų����ţ�ֻ��������
                hasOperator = true;
                break;
            }
        }
        // ���������Ԫ��"a"�ǺϷ��ģ�����ַ������������"abc"�ǲ��Ϸ���
        if (!hasOperator && formula.size() > 1) {
            cout << "����: ��ʽȱ�������! ��Ϊ���������Ԫ�����뵥���ַ�" << endl;
            continue;
        }

        Map_ic proposition_set = getProposition(formula);
        cout << "��ʽ�е������Ԫ����Ϊ: " << proposition_set.size() << endl << "��ֵ������: " << endl;
        for (unsigned int i = 0; i < proposition_set.size(); i++)
        {
            cout << proposition_set[i] << "\t";
        }
        cout << formula << endl;

        int* m;
        m = (int*)malloc(sizeof(int) * pow2(proposition_set.size()));   //�洢���鸳ֵ�����⹫ʽ����ֵ(1��0)

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
        cout << "�����⹫ʽ����ȡ��ʽΪ: " << endl;
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

        cout << "�����⹫ʽ�ĺ�ȡ��ʽΪ: " << endl;
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

        free(m);  // �ͷŶ�̬������ڴ�

        cout << "\n�Ƿ��������?��y/n��";
        cin >> continueFlag;
    } while (continueFlag == 'y' || continueFlag == 'Y');

    cout << "��лʹ�ã��������˳���" << endl;
    return 0;
}
bool checkConsecutiveLetters(const string& formula) {
    for (size_t i = 1; i < formula.length(); ++i) {
        // �жϵ�ǰ�ַ���ǰһ���ַ��Ƿ�����ĸ
        bool prevIsLetter = ((formula[i - 1] >= 'a' && formula[i - 1] <= 'z') ||
            (formula[i - 1] >= 'A' && formula[i - 1] <= 'Z'));
        bool currIsLetter = ((formula[i] >= 'a' && formula[i] <= 'z') ||
            (formula[i] >= 'A' && formula[i] <= 'Z'));
        if (prevIsLetter && currIsLetter) {
            cout << "���󣺴���������ĸ��������ĸ�������������������롣" << endl;
            return false;
        }
    }
    return true;
}
// ����Ƿ�Ϊ�չ�ʽ
bool isEmptyFormula(const string& formula) {
    return formula.empty();
}
bool checkIllegalCharacters(const string& formula) {
    // ������ʽ�е�ÿ���ַ�
    for (char c : formula) {
        // �Ϸ��ַ��жϣ�3��Ϸ����
        bool isLegal = false;
        // 1. �����Ԫ��a-z �� A-Z��
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            isLegal = true;
        }
        // 2. ������������! & | ^ ~��
        else if (c == '!' || c == '&' || c == '|' || c == '^' || c == '~') {
            isLegal = true;
        }
        // 3. ��������ţ�()��
        else if (c == '(' || c == ')') {
            isLegal = true;
        }

        // �����ڷǷ��ַ���������ʾ������false
        if (!isLegal) {
            cout << "���󣺼�⵽�Ƿ����ţ�����������" << endl;
            return false;
        }
    }
    // �����ַ����Ϸ�
    return true;
}
// �������ƥ��
bool checkParentheses(const string& formula) {
    stack<char> st;
    for (char c : formula) {
        if (c == '(') {
            st.push(c);
        }
        else if (c == ')') {
            if (st.empty() || st.top() != '(') {
                return false; // �����Ź����ƥ��
            }
            st.pop();
        }
    }
    return st.empty(); // ȷ�����������Ŷ���ƥ��
}

// ��������λ�úϷ���
bool checkOperatorPosition(const string& formula) {
    // ���ַ�������˫Ŀ�����������!��(��
    if (priority.count(formula[0]) && formula[0] != '!' && formula[0] != '(') {
        return false;
    }

    // β�ַ������������������)��
    char last = formula.back();
    if (priority.count(last) && last != ')') {
        return false;
    }

    // ��������֮�䡢�����������֮���λ�ù�ϵ
    for (size_t i = 1; i < formula.size(); ++i) {
        char prev = formula[i - 1];
        char curr = formula[i];

        // �����ź�����˫Ŀ�����
        if (prev == '(' && priority.count(curr) && curr != '!' && curr != '(') {
            return false;
        }

        // �����ź����������Ԫ��������
        if (prev == ')' && ((curr >= 'a' && curr <= 'z') ||
            (curr >= 'A' && curr <= 'Z') || curr == '(')) {
            return false;
        }

        // ˫Ŀ�����ǰ���ܶ��������
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
            cout << "����: ����δ�����ַ� '" << c << "'" << endl;
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