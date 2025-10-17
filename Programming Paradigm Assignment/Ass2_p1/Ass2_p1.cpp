//2450721 古贺 终于后悔反省了
#include <iostream>
#include <string>
using namespace std;

bool check(string& reverse)
{
	for (char c : reverse)
	{
		if (c < 'a' || c>'z')
			return false;
	}
	return true;
}
void edit_palindrome(string& reverse, string::iterator start, string::iterator end)
{
	while (start<end)
	{
		if (*start > *end)
		{
			*start = *end;
		}
		else if(*start < *end)
			*end = *start;
		start++;
		end--;
	}
}
int main()
{
	string reverse;
	while(true)
	{
		cout << "Input a string, which should contain only lowercase letters\n";
		getline(cin, reverse);
		if (reverse.empty())
		{
			cout << "Empty string entered, please re-enter\n";
		}
		if (!check(reverse))
		{
			cout << "Please re-enter, the string should contain only lowercase letters\n";
			continue;
		}
		else
			break;
	}
	cout << "Original string is:\n" << reverse << endl;
	string::iterator start = reverse.begin();
	string::iterator end = reverse.end()-1;
	edit_palindrome(reverse, start, end);
	cout << "The reversed one is:\n" << reverse << endl;

	return 0;
}
