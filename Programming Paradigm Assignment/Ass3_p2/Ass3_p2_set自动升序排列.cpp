//2458620 古贺 终于后悔反省了
#include <iostream>
#include<vector>
#include<set>
using namespace std;

bool inputNum(long long& num) {
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
void inputVector(vector<long long>& arr)
{
	while (1)
	{
		long long temp;
		if (!inputNum(temp))
			break;
		arr.push_back(temp);
	}
}
//set自动升序排列
bool isSmallerThanK(vector<long long>& arr,int k,int t)
{
	int n = arr.size();
	set<long long> window;
	for (int i = 0; i < n; ++i)
	{
		set <long long>::iterator it = window.lower_bound(arr[i] - t);//左边界
		if (it != window.end() && *it <= arr[i] + t)//是否符合右边界
			return true;
		else
			window.insert(arr[i]);
		if (window.size() > k)
			window.erase(arr[i - k]);
	}
	return false;
}
int main()
{
	long long k, t;
	vector<long long> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input k,t:\n";
	inputNum(k);
	inputNum(t);
	if (isSmallerThanK(arr, k,t))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
