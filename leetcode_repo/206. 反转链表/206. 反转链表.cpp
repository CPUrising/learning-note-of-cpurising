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

        ListNode* prev = nullptr;    // 前一个节点
        ListNode* curr = head;       // 当前节点
        ListNode* next = nullptr;    // 下一个节点

        while (curr != nullptr) {
            next = curr->next;  // 保存下一个节点
            curr->next = prev;  // 反转当前节点的指针
            prev = curr;        // 前一个节点后移
            curr = next;        // 当前节点后移
        }
        return prev;
    }
};