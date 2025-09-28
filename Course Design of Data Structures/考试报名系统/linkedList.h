#pragma once
#include<iostream>
#include<iomanip>
const int SPACE = 15;
class LinkedNode
{
public:
	std::string number;
	std::string name;
	bool sex;
	int age;
	std::string application;
	LinkedNode* next;
	LinkedNode();
	LinkedNode(std::string number, std::string name, bool sex, int age, std::string application);
	LinkedNode(const LinkedNode& other);
	LinkedNode& operator=(const LinkedNode& other);
};

bool safe_read_node(std::istream& is, LinkedNode& node);
bool safe_read_num(std::istream& is, std::string name);

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
