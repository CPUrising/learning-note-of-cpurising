#include <iostream>
using namespace std;

// 链表节点定义
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

// 辅助函数：创建新节点
ListNode* createNode(int val) {
    return new ListNode(val);
}

// 辅助函数：打印链表
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

// 测试函数
void testCase1() {
    // 创建链表A: 4 -> 1 ->
    ListNode* headA = createNode(4);
    headA->next = createNode(1);

    // 创建链表B: 5 -> 0 -> 1 ->
    ListNode* headB = createNode(5);
    headB->next = createNode(0);
    headB->next->next = createNode(1);

    // 创建相交部分: 8 -> 4 -> 5 ->
    ListNode* intersect = createNode(8);
    intersect->next = createNode(4);
    intersect->next->next = createNode(5);

    // 连接相交部分
    headA->next->next = intersect;
    headB->next->next->next = intersect;

    cout << "测试用例1:" << endl;
    cout << "链表A: ";
    printList(headA);
    cout << "链表B: ";
    printList(headB);

    Solution solution;
    ListNode* result = solution.getIntersectionNode(headA, headB);

    if (result != nullptr) {
        cout << "相交节点值为: " << result->val << endl;
    }
    else {
        cout << "两个链表不相交" << endl;
    }
    cout << endl;
}

void testCase2() {
    // 创建不相交的两个链表
    // 链表A: 1 -> 2 -> 3
    ListNode* headA = createNode(1);
    headA->next = createNode(2);
    headA->next->next = createNode(3);

    // 链表B: 4 -> 5 -> 6
    ListNode* headB = createNode(4);
    headB->next = createNode(5);
    headB->next->next = createNode(6);

    cout << "测试用例2:" << endl;
    cout << "链表A: ";
    printList(headA);
    cout << "链表B: ";
    printList(headB);

    Solution solution;
    ListNode* result = solution.getIntersectionNode(headA, headB);

    if (result != nullptr) {
        cout << "相交节点值为: " << result->val << endl;
    }
    else {
        cout << "两个链表不相交" << endl;
    }
    cout << endl;
}

int main() {
    testCase1();  // 有相交节点的情况
    testCase2();  // 无相交节点的情况

    return 0;
}
