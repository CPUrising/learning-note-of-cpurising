#include <iostream>
#include <vector>
#include <string>
using namespace std;

// ����Solution��Ķ����ڴ˴�
class Solution {
public:
    int evalRPN(vector<string>& tokens) {
        // ��ʵ�ֵ��沨�����ʽ��ֵ�߼�
        return 0; // ��ʱ����ֵ
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