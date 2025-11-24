//2458620 古贺 终于后悔反省了
#include <iostream>
#include <vector>
using namespace std;

class MinHeap
{
private:

public:

};

void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good())
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Please re-enter a positive integer:\n";
		}
		else
			break;
	}
}

void inputBlock(vector<int>&block,int N)
{
	while (N--)
	{
		int temp;
		inputNum(temp);
		block.push_back(temp);
	}
}

int main()
{
	int N;
	vector<int>block;
	inputNum(N);


	return 0;
}
