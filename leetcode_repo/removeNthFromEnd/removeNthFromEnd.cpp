//https://leetcode.cn/problems/remove-nth-node-from-end-of-list/description/
#include <iostream>
using namespace std;

// 链表节点定义
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode dummyHead;
        dummyHead.next = head;
        ListNode* slow = &dummyHead;
        ListNode* fast = slow+n;

    }
};

// 创建链表的辅助函数
ListNode* createList(int arr[], int size) {
    if (size == 0) return nullptr;

    ListNode* head = new ListNode(arr[0]);
    ListNode* current = head;

    for (int i = 1; i < size; ++i) {
        current->next = new ListNode(arr[i]);
        current = current->next;
    }

    return head;
}

// 打印链表的辅助函数
void printList(ListNode* head) {
    ListNode* current = head;
    while (current != nullptr) {
        cout << current->val;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

// 释放链表内存的辅助函数
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// 测试函数
void testCase(int arr[], int size, int n) {
    ListNode* head = createList(arr, size);
    cout << "原始链表: ";
    printList(head);

    Solution solution;
    ListNode* result = solution.removeNthFromEnd(head, n);

    cout << "删除倒数第" << n << "个节点后: ";
    printList(result);
    cout << "-------------------------" << endl;

    freeList(result);
}

int main() {
    // 测试用例1: 正常情况
    int arr1[] = { 1, 2, 3, 4, 5 };
    testCase(arr1, 5, 2);

    // 测试用例2: 删除头节点
    int arr2[] = { 1, 2 };
    testCase(arr2, 2, 2);

    // 测试用例3: 删除尾节点
    int arr3[] = { 1, 2, 3 };
    testCase(arr3, 3, 1);

    // 测试用例4: 单节点链表
    int arr4[] = { 5 };
    testCase(arr4, 1, 1);

    return 0;
}