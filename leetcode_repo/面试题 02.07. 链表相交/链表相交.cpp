#include <iostream>
using namespace std;

// ����ڵ㶨��
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(NULL) {}
};

class Solution {
public:
    ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
        ListNode* pA = headA;
        ListNode* pB = headB;
        if (headA == nullptr || headB == nullptr)
            return nullptr;
        int sizeA = 0;
        int sizeB = 0;
        while (pA != nullptr)
        {
            pA = pA->next;
            sizeA++;
        }
        while (pB != nullptr)
        {
            pB = pB->next;
            sizeB++;
        }
        pA = headA;
        pB = headB;
        if (sizeA >= sizeB)
        {
            while (sizeA > sizeB)
            {
                pA = pA->next;
                sizeA--;
            }
        }
        else
        {
            while (sizeA < sizeB)
            {
                pB = pB->next;
                sizeB--;
            }
        }
        while (pA!= pB&&pA!=nullptr)
        {
            pA = pA->next;
            pB = pB->next;
        }
        if (pA == nullptr)
            return nullptr;
        else
            return pA;
    }
};

// ���������������½ڵ�
ListNode* createNode(int val) {
    return new ListNode(val);
}

// ������������ӡ����
void printList(ListNode* head) {
    ListNode* curr = head;
    while (curr != nullptr) {
        cout << curr->val;
        if (curr->next != nullptr) {
            cout << " -> ";
        }
        curr = curr->next;
    }
    cout << endl;
}

// ���Ժ���
void testCase1() {
    // ��������A: 4 -> 1 ->
    ListNode* headA = createNode(4);
    headA->next = createNode(1);

    // ��������B: 5 -> 0 -> 1 ->
    ListNode* headB = createNode(5);
    headB->next = createNode(0);
    headB->next->next = createNode(1);

    // �����ཻ����: 8 -> 4 -> 5 ->
    ListNode* intersect = createNode(8);
    intersect->next = createNode(4);
    intersect->next->next = createNode(5);

    // �����ཻ����
    headA->next->next = intersect;
    headB->next->next->next = intersect;

    cout << "��������1:" << endl;
    cout << "����A: ";
    printList(headA);
    cout << "����B: ";
    printList(headB);

    Solution solution;
    ListNode* result = solution.getIntersectionNode(headA, headB);

    if (result != nullptr) {
        cout << "�ཻ�ڵ�ֵΪ: " << result->val << endl;
    }
    else {
        cout << "���������ཻ" << endl;
    }
    cout << endl;
}

void testCase2() {
    // �������ཻ����������
    // ����A: 1 -> 2 -> 3
    ListNode* headA = createNode(1);
    headA->next = createNode(2);
    headA->next->next = createNode(3);

    // ����B: 4 -> 5 -> 6
    ListNode* headB = createNode(4);
    headB->next = createNode(5);
    headB->next->next = createNode(6);

    cout << "��������2:" << endl;
    cout << "����A: ";
    printList(headA);
    cout << "����B: ";
    printList(headB);

    Solution solution;
    ListNode* result = solution.getIntersectionNode(headA, headB);

    if (result != nullptr) {
        cout << "�ཻ�ڵ�ֵΪ: " << result->val << endl;
    }
    else {
        cout << "���������ཻ" << endl;
    }
    cout << endl;
}

int main() {
    testCase1();  // ���ཻ�ڵ�����
    testCase2();  // ���ཻ�ڵ�����

    return 0;
}
