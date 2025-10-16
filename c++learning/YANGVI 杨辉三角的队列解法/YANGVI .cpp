//2450721 古贺 终于后悔反省了
#include <iostream>
#include<queue>
using namespace std;
int main()
{
	queue<int> Yang;
	Yang.push(0);
	Yang.push(1);
	Yang.push(1);

	int row = 5;//set row
	cout << 1 << endl;
	for (int i = 1; i < row; i++)
	{
		Yang.push(0);
		int s = Yang.front();
		for (int j = 0; j < 2 + i;j++)
		{
			Yang.pop();
			int t= Yang.front();
			Yang.push(s + t);
			s = t;
			cout << Yang.back() << ' ';
		}
		cout << endl;
	}

	return 0;
}
