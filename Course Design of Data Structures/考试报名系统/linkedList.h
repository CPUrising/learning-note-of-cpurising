#pragma once
#include<iostream>
#include<iomanip>
using namespace std;
const int SPACE = 15;
class LinkedNode
{
public:
	string number;
	string name;
	bool sex;
	int age;
	string application;
	LinkedNode* next;
	LinkedNode();
	LinkedNode(string number, string name, bool sex, int age, std::string application);
	LinkedNode(const LinkedNode& other);
	LinkedNode& operator=(const LinkedNode& other);
};

bool safe_read_node(istream& is, LinkedNode& node);
bool safe_read_num(istream& is, string& name);

class Linkedlist
{
private:
	int _size;
	LinkedNode* _dummyHead;
public:
	Linkedlist();
	~Linkedlist();
	int get_size();
	void print_head_row();
	void print_person(const LinkedNode& node);
	LinkedNode* node_search(string num);//search "position"
	void node_find(string num);//find and print
	void node_putback(const LinkedNode& node);
	void node_insert(int index, const LinkedNode& node);//insert at "index",rather than"number"
	void node_delete(string num);//delete at "number"
	void node_edit(string num, const LinkedNode& node);//edit at "number"
	void print_list();
};
