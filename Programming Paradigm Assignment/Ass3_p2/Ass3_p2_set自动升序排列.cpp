//2458620 古贺 终于后悔反省了
//问题 2：
//
//给定一个整数数组 nums 和两个整数 k 和 t，判断是否存在两个不同的索引 i 和 j，使得：
//
//abs(i - j) <= k
//
//abs(nums \[i] - nums \[j]) <= t
//
//示例 1：
//
//输入：nums = \[1, 5, 9, 3, 1]，k = 2，t = 2
//
//输出：true
//
//解释：索引 1 和 3：abs(1 - 3) = 2 <= 2，abs(5 - 3) = 2 <= 2
//
//示例 2：
//
//输入：nums = \[1, 10, 20, 30, 40]，k = 2，t = 5
//
//输出：false
//
//解释：在任何长度为 3 的滑动窗口中，任意两个元素之间的差值都大于 5。
//
//提示：
//
//1 <= nums.length <= 10 ^ 4
//
//- 10 ^ 9 <= nums \[i]、nums \[j] <= 10 ^ 9
//
//0 <= k <= 10000
//
//0 <= t <= 10 ^ 9
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
