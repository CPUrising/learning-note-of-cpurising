#include<iostream>
class LinkedNode
{
public:
    int num;
    LinkedNode* next;
    LinkedNode(int num);
};
LinkedNode::LinkedNode(int num=0)
{
    this->num = num;
    this->next = nullptr;
}

class Linkedlist
{
public:
    LinkedNode* _dummyHead;
    Linkedlist()
    {
        _dummyHead = new LinkedNode;
        _dummyHead->next = _dummyHead;
    }
    ~Linkedlist()
    {
        LinkedNode* cur = _dummyHead;
        while (cur != nullptr)
        {
            LinkedNode* temp = cur;
            cur = cur->next;
            delete temp;
        }
    }
    void node_pushback(int num);
    void node_pushback_loop(int num);
    void node_delete(int target);
};
void Linkedlist::node_pushback(int num)
{
    LinkedNode* cur = _dummyHead;
    while (cur->next != nullptr)
    {
        cur = cur->next;
    }
    LinkedNode* newNode = new LinkedNode(num);
    cur->next = newNode;
}
void Linkedlist::node_pushback_loop(int n)
{
    LinkedNode* cur = _dummyHead;
    while (n--)
    {
        node_pushback(n);
    }
}
void Linkedlist::node_delete(int target)
{
    LinkedNode* cur = _dummyHead;
    if (cur->next == _dummyHead)
        return;
    while (target--)
    {
        cur = cur->next;
    }
    LinkedNode* temp = cur->next;
    cur->next = cur->next->next;
    delete temp;
    temp = nullptr;
}

class Solution {
public:
    int iceBreakingGame(int num, int target) 
    {
        Linkedlist ibg;
        ibg.node_pushback_loop(num);
    }
};