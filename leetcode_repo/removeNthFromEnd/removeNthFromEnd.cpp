//https://leetcode.cn/problems/remove-nth-node-from-end-of-list/description/
#include <iostream>
using namespace std;

// ����ڵ㶨��
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

// ��������ĸ�������
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

// ��ӡ����ĸ�������
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

// �ͷ������ڴ�ĸ�������
void freeList(ListNode* head) {
    while (head != nullptr) {
        ListNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// ���Ժ���
void testCase(int arr[], int size, int n) {
    ListNode* head = createList(arr, size);
    cout << "ԭʼ����: ";
    printList(head);

    Solution solution;
    ListNode* result = solution.removeNthFromEnd(head, n);

    cout << "ɾ��������" << n << "���ڵ��: ";
    printList(result);
    cout << "-------------------------" << endl;

    freeList(result);
}

int main() {
    // ��������1: �������
    int arr1[] = { 1, 2, 3, 4, 5 };
    testCase(arr1, 5, 2);

    // ��������2: ɾ��ͷ�ڵ�
    int arr2[] = { 1, 2 };
    testCase(arr2, 2, 2);

    // ��������3: ɾ��β�ڵ�
    int arr3[] = { 1, 2, 3 };
    testCase(arr3, 3, 1);

    // ��������4: ���ڵ�����
    int arr4[] = { 5 };
    testCase(arr4, 1, 1);

    return 0;
}