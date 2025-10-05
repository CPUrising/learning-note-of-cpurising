#include<iostream>
class LinkedNode
{
public:
    int num;
    LinkedNode* next;
    LinkedNode(int num);
};

LinkedNode::LinkedNode(int num)
{
    this->num = num;
    this->next = nullptr;
}

class Linkedlist
{
public:
    LinkedNode* _dummyHead;
    int _size;  // 新增：记录节点数量

    Linkedlist()
    {
        _dummyHead = new LinkedNode(0);
        _dummyHead->next = _dummyHead;  // 初始化为环形
        _size = 0;
    }

    ~Linkedlist()
    {
        LinkedNode* cur = _dummyHead->next;
        while (cur != _dummyHead)  // 遍历环形链表
        {
            LinkedNode* temp = cur;
            cur = cur->next;
            delete temp;
        }
        delete _dummyHead;  // 最后删除虚拟头节点
        _dummyHead = nullptr;
    }

    // 在环形链表末尾添加节点
    void node_pushback(int num)
    {
        LinkedNode* newNode = new LinkedNode(num);
        LinkedNode* cur = _dummyHead;

        // 找到最后一个节点（next指向dummyHead的节点）
        while (cur->next != _dummyHead)
        {
            cur = cur->next;
        }

        cur->next = newNode;
        newNode->next = _dummyHead;  // 保持环形结构
        _size++;
    }

    // 批量添加节点
    void node_pushback_loop(int n)
    {
        for (int i = 0; i < n; i++)  // 从0开始编号更符合实际游戏场景
        {
            node_pushback(i);
        }
    }

    // 删除从当前位置开始的第target个节点，返回删除节点的下一个节点
    LinkedNode* node_delete(LinkedNode* start, int target)
    {
        if (_size == 1) return nullptr;

        // 找到要删除节点的前一个节点
        LinkedNode* cur = start;
        for (int i = 1; i < target-1; i++)
        {
            cur = cur->next;
            if (cur == _dummyHead)  // 跳过虚拟头节点
                i--;  // 重新计数
        }

        // 要删除的节点 ,无法删除target=1时  0 node
        if (target == 1)
            cur = _dummyHead;
        LinkedNode* temp;
        temp = cur->next;
        if (temp == _dummyHead)  // 如果指向虚拟头节点，说明要删除第一个有效节点
        {
            temp = temp->next;
            cur = cur->next;
        }

        cur->next = temp->next;  // 移除节点
        int deletedNum = temp->num;
        delete temp;
        _size--;

        return cur->next;  // 返回下一个开始计数的节点
    }

    // 获取第一个有效节点
    LinkedNode* getFirstNode()
    {
        return _dummyHead->next;
    }
};

class Solution {
public:
    int iceBreakingGame(int num, int target)
    {
        if (num < 0 || target < 0) return -1;  // 边界检查

        Linkedlist ibg;
        ibg.node_pushback_loop(num);

        LinkedNode* current = ibg.getFirstNode();

        // 循环删除，直到只剩一个节点
        while (ibg._size > 1)
        {
            current = ibg.node_delete(current, target);
            // 如果下一个是虚拟头节点，就再移一步
            if (current == ibg._dummyHead)
                current = current->next;
        }

        return current->num;  // 返回最后剩下的节点编号
    }
};
int main()
{
    Solution solution4;
    int result4 = solution4.iceBreakingGame(5, 1);  // 经典约瑟夫环问题：7人，数到4淘汰
    std::cout << "最后剩下的编号是: " << result4 << std::endl;  // 应该输出4

    Solution solution2;
    int result2 = solution2.iceBreakingGame(7, 4);  // 经典约瑟夫环问题：7人，数到4淘汰
    std::cout << "最后剩下的编号是: " << result2 << std::endl;  // 应该输出3

    Solution solution1;
    int result1 = solution1.iceBreakingGame(5, 2);  // 经典约瑟夫环问题：5人，数到2淘汰
    std::cout << "最后剩下的编号是: " << result1 << std::endl;  // 应该输出3

    Solution solution3;
    int result3 = solution3.iceBreakingGame(12, 5);
    std::cout << "最后剩下的编号是: " << result3 << std::endl;  // 应该输出4

    return 0;
}