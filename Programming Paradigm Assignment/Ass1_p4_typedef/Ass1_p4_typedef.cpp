//2450721 古贺 终于后悔反省了
#include <iostream>
using namespace std;
struct Practice
{
public:
	string s1;
	string s2;
	int n;
	Practice();
	void print_Practice();
};
Practice::Practice()
{
	s1 = "assignment";
	s2 = " problem ";
	n = 3;
}
void Practice::print_Practice()
{
	cout << s1 << s2 << n;
}
int main()
{
	typedef struct Practice pra;
	pra obj;
	pra* p = &obj;
	p->s1 = "Programming Paradigm ";
	p->s2 = "Assignment Problem ";
	p->n = 4;
	p->print_Practice();
	return 0;
}
