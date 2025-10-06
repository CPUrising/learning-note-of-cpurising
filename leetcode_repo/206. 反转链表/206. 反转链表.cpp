#include<iostream>
struct ListNode {
    int val;
    ListNode* next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}
};
 
class Solution {
public:
    ListNode* reverseList(ListNode* head) {
        if (head == nullptr || head->next == nullptr) {
            return head;
        }

        ListNode* prev = nullptr;    // ǰһ���ڵ�
        ListNode* curr = head;       // ��ǰ�ڵ�
        ListNode* next = nullptr;    // ��һ���ڵ�

        while (curr != nullptr) {
            next = curr->next;  // ������һ���ڵ�
            curr->next = prev;  // ��ת��ǰ�ڵ��ָ��
            prev = curr;        // ǰһ���ڵ����
            curr = next;        // ��ǰ�ڵ����
        }
        return prev;
    }
};