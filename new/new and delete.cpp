#include"head.h"
Box::Box()
{
	cout << "initializing......" << endl;
	a = 1;
	b = 2;
	c = 3;
	cout << a << ' ' << b << ' ' << c << ' ' << endl;

	cout << "done!" << endl;
}
Box::~Box()
{
	a = 3;
	b = 2;
	c = 1;
	cout << "emmmm" << endl;
	cout << a << ' ' << b << ' ' << c << ' ' << endl;
}
int main()
{
	Box *boxarray=new Box[2];
}