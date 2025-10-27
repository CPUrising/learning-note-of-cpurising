//2458620 古贺 终于后悔反省了
#include <iostream>
#include<vector>
#include <unordered_set>
using namespace std;

bool isIncludeRepeat(vector<int>& arr, int k)
{
	int len = arr.size();
	if (len < k)
		return false;

	unordered_set<int> window;
	for (int i = 0; i < k; ++i)
	{
		if (window.find(arr[i]) != window.end())
			return true;
		else
		{
			window.insert(arr[i]);
		}
	}



}
void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good())
		{
			cin.ignore(1000, '\n');
			cout << "Please re-enter a integer:\n";
		}
		else
			break;
	}
}
void inputArray()
{

}
int main()
{

	return 0;
}
