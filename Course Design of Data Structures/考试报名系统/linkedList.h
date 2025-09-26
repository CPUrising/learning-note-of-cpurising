#pragma once
#include<iostream>
class LinkedNode
{
public:
	std::string number;
	std::string name;
	bool sex;
	int age;
	std::string application;
	LinkedNode* next;
	LinkedNode(std::string number, std::string name, bool sex, int age, std::string application) :
				number(number), name(name), sex(sex),age(age), application(application), next(NULL) { }
};
class Linkedlist
{
private:
	int _size;
	LinkedNode* _dummyHead;
public:
	Linkedlist();
	~Linkedlist();
	int get(int index);
	int find(int value);
	void node_putback_(int value);
	void node_insert(int index, int value);//change at certain index
	void node_delete(int index);//change at certain index
	void node_modify(int index);
	void print_list();
};