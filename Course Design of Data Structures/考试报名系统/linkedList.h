#pragma once
#include<iostream>
#include<iomanip>
const int SPACE = 2;
class LinkedNode
{
public:
	std::string number;
	std::string name;
	bool sex;
	int age;
	std::string application;
	LinkedNode* next;
	LinkedNode()
	{
		number = "0721";
		name = "noa";
		sex = 0;
		age = 86;
		application = "blue archive";
		next = nullptr;
	}
	LinkedNode(std::string number, std::string name, bool sex, int age, std::string application) :
				number(number), name(name), sex(sex),age(age), application(application), next(NULL) { }
	LinkedNode(const LinkedNode& other) {
		// Copy basic members (value copy)
		number = other.number;
		name = other.name;
		sex = other.sex;
		age = other.age;
		application = other.application;
		// Deep copy for the next pointer: only copy the current node itself, set next to null (critical!)
		// Reason: When the new node is added to the linked list, its next pointer should be managed 
		// by the linked list itself to avoid association with the original linked list.
		next = nullptr;  //instead of "next = other.next "(shallow copy would cause pointer confusion)
	}
	LinkedNode& operator=(const LinkedNode& other) {
		if (this != &other) {  // avoid self copy
			number = other.number;
			name = other.name;
			sex = other.sex;
			age = other.age;
			application = other.application;
			// 这里没有复制next指针，姑且以后处理。应该是深拷贝然后next=other.next?
		}
		return *this;
	}
};
std::istream& operator>>(std::istream& is, LinkedNode& node) {
	is >> node.number;
	is >> node.name;
	is >> node.sex;
	is >> node.age;
	is >> node.application;
	// 不需要处理next指针，输入操作通常只处理当前节点的数据
	return is;  // 返回输入流，支持链式输入
}
class Linkedlist
{
private:
	int _size;
	LinkedNode* _dummyHead;
public:
	Linkedlist();
	~Linkedlist();
	void print_head_row();
	void print_person(const LinkedNode& node);
	LinkedNode* node_search(std::string num);//search "position"
	void node_find(std::string num);//find and print
	void node_putback(const LinkedNode& node);
	void node_insert(int index, const LinkedNode& node);//insert at "index",rather than"number"
	void node_delete(std::string num);//delete at "number"
	void node_edit(std::string num, const LinkedNode& node);//edit at "number"
	void print_list();
};
