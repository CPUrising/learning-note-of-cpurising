#include"linkedList.h"

Linkedlist::Linkedlist()
{
	_dummyHead = new LinkedNode(0);
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
int Linkedlist::get(int index)//index starts from 0
{
	if (index >= _size || index < 0)
		return -1;
	LinkedNode* cur = _dummyHead->next;
	while (index--)
		cur = cur->next;
	return cur->val;
}
int Linkedlist::find(int value)
{
	LinkedNode* cur = _dummyHead->next;
	int count = 0;
	while (cur != nullptr)
	{
		if (cur->val == value)
			return count;
		cur = cur->next;
		count++;
	}
	return -1;
}
void Linkedlist::putback_node(int value)
{
	LinkedNode* cur = _dummyHead;
	while (cur->next != nullptr)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(value);
	cur->next = newNode;
	_size++;
}
void Linkedlist::insert_node(int index, int value)
{
	LinkedNode* cur = _dummyHead;
	if (index < 0 || index >= _size)
		index = 0;
	while (index--)
	{
		cur = cur->next;
	}
	LinkedNode* newNode = new LinkedNode(value);
	newNode->next = cur->next;
	cur->next = newNode;
	_size++;
}
void Linkedlist::delete_node(int index)
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
	//delete����ָʾ�ͷ���tmpָ��ԭ����ָ���ǲ����ڴ棬
		//��delete���ָ��tmp��ֵ����ַ�����Ǿ���NULL���������ֵ��Ҳ���Ǳ�delete��
		//������ټ���һ��tmp=nullptr,tmp���Ϊ��ָ��Ұָ��
		//���֮��ĳ���С��ʹ����tmp����ָ������Ԥ����ڴ�ռ�
	temp = nullptr;
	_size--;
}
void Linkedlist::print_list()
{
	LinkedNode* cur = _dummyHead->next;
	while (cur != nullptr)
	{
		std::cout << cur->val << ' ';
		cur = cur->next;
	}
}