//2458620 古贺 终于后悔反省了
#include <iostream>
#include<vector>
#include <unordered_set>
using namespace std;

bool isIncludeRepeat(vector<int>& arr, int k)
{
	int len = arr.size();
	if (len < k||k<=0)
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

	for (int i = k; i < len; ++i)
	{
		window.erase(arr[i - k]);
		if (window.find(arr[i]) != window.end())
			return true;
		else
		{
			window.insert(arr[i]);
		}
	}
	return false;
}
bool inputNum(int& num) {
	cin >> num;
	// 情况1：输入是整数（包括正数、负数、0）
	if (!cin.fail()) {
		return true;
	}
	// 情况2：输入非整数（如'q'），视为结束标志
	else {
		cin.clear(); // 清除错误状态
		cin.ignore(1000, '\n'); // 忽略缓冲区剩余内容
		return false;
	}
}
void inputVector(vector<int> &arr)
{
	while (1)
	{
		int temp;
		if(!inputNum(temp))
			break;
		arr.push_back(temp);
	}
}
int main()
{
	int k;
	vector<int> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input the length of the window:\n";
	inputNum(k);
	if (isIncludeRepeat(arr, k))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
