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
	std::cout << "考号" << std::setw(SPACE) << "姓名" << std::setw(SPACE) << "性别" << std::setw(SPACE) << "年龄" << std::setw(SPACE) << "报考类别\n";
}
void Linkedlist::print_person(const LinkedNode& node)
{
	std::cout << node.number << std::setw(SPACE) << node.name << std::setw(SPACE) << node.sex << std::setw(SPACE) << node.age << std::setw(SPACE) << node.application << std::endl;
}
LinkedNode* Linkedlist::node_search(std::string num)//index starts from 0
{
	LinkedNode* cur = _dummyHead->next;
	while (cur != nullptr&&cur->number!= num)
		cur = cur->next;
	if (cur == nullptr)
	{
		return nullptr;
	}
	return cur;
}
void Linkedlist::node_find(std::string num)
{
	LinkedNode* cur = node_search(num);
	if (cur == nullptr)
		std::cout << "查无此人\n";
	else
	{
		print_head_row();
		print_person(*cur);
	}
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
	LinkedNode* newNode = new LinkedNode(node);
	newNode->next = cur->next;
	cur->next = newNode;
	_size++;
}
void Linkedlist::node_delete(std::string num)
{
	LinkedNode* cur = node_search(num);
	if (cur == nullptr)
		std::cout << "查无此人\n";
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
void Linkedlist::node_edit(std::string num, const LinkedNode& node)
{
	LinkedNode* cur = node_search(num);
	if (cur == nullptr)
		std::cout << "查无此人\n";
	*(cur->next) = node;//well ,I need overload '=' here
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