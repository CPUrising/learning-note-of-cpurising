/**https://leetcode.cn/problems/linked-list-cycle-ii/
 Definition for singly-linked list.*/
#include<iostream>
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};
class Solution {
public:
    ListNode* detectCycle(ListNode* head) {
        if (head==nullptr||head->next == nullptr|| head->next->next == nullptr)
            return nullptr;

        ListNode* slow = head;
        ListNode* fast = head;
        do {
            if (fast == nullptr || fast->next == nullptr)
                return nullptr;
            slow = slow->next;          
            fast = fast->next->next;   
        } while (slow != fast);
        ListNode* detect = head;
        while (detect!=slow)
        {
            slow = slow->next;
            detect = detect->next;
        }
        return detect;
    }
};