//2450721 �ź� ���ں�ڷ�ʡ��
#include <iostream>
using namespace std;
void print_star(string*s)
{
	cout << "print_star " << *s << endl;
}
void print_reference(string& s)
{
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
