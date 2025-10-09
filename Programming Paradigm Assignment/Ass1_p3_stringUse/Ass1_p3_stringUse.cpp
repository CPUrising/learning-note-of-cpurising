//2450721 古贺 终于后悔反省了
#include <iostream>
using namespace std;
void print_star(string*s)
{
	(*s)[1] = 'H';
	cout << "print_star " << *s << endl;
}
void print_reference(string& s)
{
	s[2] = 'I';
	cout << "print_reference " << s << endl;
}
int main()
{
	string s = "This is a string.";
	cout << "original " << s << endl;
	print_star(&s);
	print_reference(s);
	return 0;
}
