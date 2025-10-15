#include <vector>
#include<iostream>
#include<list>
using namespace std;
int main()
{
	const int SIZE = 8;
	int ia[SIZE] = { 1,2,3,4,5 };

	int* p = &ia[0];

	vector< int >::iterator it1;
	vector< int >::iterator it5;
	vector< int >::const_iterator it2;
	vector< string >::iterator it3;
	list< int >::iterator it4;

	it1 = it5;
	//it1 = p; //false
	//it1 = it2; //?false
	//it1 = it3; //?false
	//it1 = it4; //?false
}
