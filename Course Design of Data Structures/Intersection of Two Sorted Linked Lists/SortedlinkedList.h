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
	void node_pushback(int num);
	void node_pushback_loop();
	void sorted(Linkedlist& s1, Linkedlist& s2, Linkedlist& s3);
	void print_list();
};
