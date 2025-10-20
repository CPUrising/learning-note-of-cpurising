//2458620 古贺 终于后悔反省了
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
//ctrl,click function and jump,ladies and gentlemen.
void removeLeadingZero();
void multiple();
void inputString(string& s);
bool checkString(string& k);

int main()
{
	string numA;
	string numB;
	inputString(numA);
	inputString(numB);


	return 0;
}

bool checkString(string& k)
{
	for (char c : k) 
		if (!isdigit(c))
			return false;
	return true;
}
void inputString(string& k)
{
	while (1)
	{
		cin >> k;
		if (!cin.good() || !checkString(k))
		{
			cout << "Please re-enter a num:\n";
			cin.ignore(1000, '\n');
		}
		else
			break;
	}
}