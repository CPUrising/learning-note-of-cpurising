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
};
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
	void node_find(int index);//find and print
	void node_putback(const LinkedNode& node);
	void node_insert(int index, const LinkedNode& node);
	void node_delete(int index);
	void node_edit(int index, const LinkedNode& node);
	void print_list();
};
