//2458620 古贺 终于后悔反省了
#include <iostream>
using namespace std;
void input(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 2 || len > 20)
		{
			cin.ignore('\n', 1000);
			cout << "Please re-enter a positive integer (3-10000):\n";
		}
		else
			break;
	}
}
int main()
{
	int p, m;
	input(p);
	input(m);
	return 0;
}
