#include<iostream>

class LinkedNode
{
public:
	int val;
	LinkedNode* next;
	LinkedNode(int value) :val(value), next(NULL) {}
};
class Linkedlist
{
	friend LinkedNode;
private:
	int _size;
	LinkedNode* _dummyHead;
public:
	Linkedlist();
	int get(int index);
	int find(int value);
	void putback_node(int value);
	void insert_node(int index);//change at certain index
	void delete_node(int index);//change at certain index
	void print_list();
};

Linkedlist::Linkedlist()
{
	_dummyHead = new LinkedNode(0);
	_size = 0;
}
int Linkedlist::get(int index)
{
	if (index > _size - 1 || index < 0)
		return -1;
	LinkedNode* cur = _dummyHead;
	while (index--)
		cur = cur->next;
	return cur->val;
}
int Linkedlist::find(int value)
{
	LinkedNode* cur = _dummyHead;
	int count = 0;
	while (1)
	{
		if (!(cur->next))
			return -1;
		if (cur->val == value)
			return count;
		count++;
	}
}
void Linkedlist::putback_node(int value)
{
	LinkedNode* cur = _dummyHead;
	while (!(cur->next))
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(value);
	cur->next = newNode;
	_size++;
}
void Linkedlist::insert_node(int index)
{
}
void Linkedlist::delete_node(int index)
{

}
void Linkedlist::print_list()
{
	LinkedNode* cur = _dummyHead;
	while (1)
	{
		if (!(cur->next))
			return;
		std::cout << cur->val << std::endl;
	}
}
