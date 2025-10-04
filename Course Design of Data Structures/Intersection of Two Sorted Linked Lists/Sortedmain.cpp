//2450721 古贺 终于后悔反省了
#include <iostream>
#include"SortedlinkedList.h"
using namespace std;
int main()
{
	Linkedlist s1;
	Linkedlist s2;
	Linkedlist s3; 
	std::cout << "Enter elements for the first linked list:" << std::endl;
	std::cout << "Please enter integers (enter -1 to finish):" << std::endl;
	s1.node_pushback_loop();
	std::cout << "First linked list elements:";
	s1.print_list();
	std::cout << "Enter elements for the second linked list:" << std::endl;
	std::cout << "Please enter integers (enter -1 to finish):" << std::endl;
	s2.node_pushback_loop();
	std::cout << "Second linked list elements:";
	s2.print_list();
	s3.sorted(s1, s2,s3);
	std::cout << "Intersection of the two linked lists:";
	s3.print_list();
	return 0;
}
