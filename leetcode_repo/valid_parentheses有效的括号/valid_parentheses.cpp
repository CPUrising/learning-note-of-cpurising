#include <iostream>
#include <string>
#include <stack>
#include <unordered_map>
using namespace std;
//there are operations: pop()NO RETURN  push top()(==peek) empty size
class Solution {
public:
    bool isValid(string s) {
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
};
// ���Դ���
int main() {
    Solution solution;

    // ���԰���
    string testCases[] = {
        "]",       // true
        "()[]{}",   // true
        "(]",       // false
        "([])",     // true
        "([)]",     // false
        "",         // true (���ַ���)
        "({[)]}",   // false
        "({[]})"    // true
    };

    int numTests = sizeof(testCases) / sizeof(testCases[0]);

    for (int i = 0; i < numTests; i++) {
        bool result = solution.isValid(testCases[i]);
        cout << "���԰��� " << i + 1 << ": \"" << testCases[i] << "\" -> "
            << (result ? "��Ч" : "��Ч") << endl;
    }

    return 0;
}
