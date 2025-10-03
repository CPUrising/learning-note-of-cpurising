#pragma once
#include<iostream>
#include<iomanip>
const int SPACE = 15;
class LinkedNode
{
public:
	int num;
	LinkedNode* next;
	LinkedNode();
	LinkedNode(int num);
	LinkedNode(const LinkedNode& other);
	LinkedNode& operator=(const LinkedNode& other);
};

bool safe_read_node(std::istream& is, LinkedNode& node);
bool safe_read_num(std::istream& is, std::string& name);

class Linkedlist
{
private:
	int _size;
	LinkedNode* _dummyHead;
public:
	Linkedlist();
	~Linkedlist();
	LinkedNode* node_search(int num);//search "position"
	void node_find(int num);//find and print
	void node_putback(const LinkedNode& node);
	void print_list();
};
