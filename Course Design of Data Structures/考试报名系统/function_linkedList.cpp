#include"linkedList.h"

Linkedlist::Linkedlist()//I have to add too much for this dummy ,how to avoid?
{
	_dummyHead = new LinkedNode;
	_size = 0;
}
Linkedlist::~Linkedlist()
{
	LinkedNode* cur = _dummyHead;
	while (cur != nullptr)
	{
		LinkedNode* temp = cur;
		cur = cur->next;
		delete temp;
	}
}
void Linkedlist::print_head_row()
{
	std::cout << "����" << std::setw(SPACE) << "����" << std::setw(SPACE) << "�Ա�" << std::setw(SPACE) << "����" << std::setw(SPACE) << "�������\n";
}
void Linkedlist::print_person(const LinkedNode& node)
{
	std::cout << node.number << std::setw(SPACE) << node.name << std::setw(SPACE) << node.sex << std::setw(SPACE) << node.age << std::setw(SPACE) << node.application << std::endl;
}
void Linkedlist::node_find(int index)//index starts from 0
{
	if (index >= _size || index < 0)
	{
		std::cout << "���޴���\n";
		return;
	}
	LinkedNode* cur = _dummyHead->next;
	while (index--)
		cur = cur->next;
	print_head_row();
	print_person(*cur);
}
void Linkedlist::node_putback(const LinkedNode& node)
{
	LinkedNode* cur = _dummyHead;
	while (cur->next != nullptr)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(node);
	cur->next = newNode;
	_size++;
}
void Linkedlist::node_insert(int index, const LinkedNode& node)
{
	LinkedNode* cur = _dummyHead;
	if (index < 0 || index >= _size)
		index = 0;
	while (index--)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(node);
	newNode->next = cur->next;
	cur->next = newNode;
	_size++;
}
void Linkedlist::node_delete(int index)
{
	LinkedNode* cur = _dummyHead;
	if (index < 0 || index >= _size)
		return;
	while (index--)
	{
		cur = cur->next;
	}
	LinkedNode* temp = cur->next;
	cur->next = cur->next->next;
	delete temp;
	// The delete command instructs to free the memory originally pointed to by the tmp pointer.
	// After being deleted, the value (address) of pointer tmp is not NULL but a random value. That is, after deletion,
	// if we don't add tmp = nullptr, tmp will become a dangling pointer that points randomly.
	// If the program accidentally uses tmp afterwards, it will point to an unpredictable memory space.
	temp = nullptr;
	_size--;
}
void Linkedlist::node_edit(int index, const LinkedNode& node)
{
	LinkedNode* cur = _dummyHead;
	if (index < 0 || index >= _size)
		index = 0;
	while (index--)
	{
		cur = cur->next;
	}
	cur->next=node//well ,I need overload '=' here
}
void Linkedlist::print_list()
{
	LinkedNode* cur = _dummyHead->next;
	print_head_row();
	while (cur != nullptr)
	{
		print_person(*cur);
		cur = cur->next;
	}
}