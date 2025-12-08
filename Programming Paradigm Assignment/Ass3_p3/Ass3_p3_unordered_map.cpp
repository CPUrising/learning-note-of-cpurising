//2458620 古贺 终于后悔反省了
//问题 3：
//
//给定一个整数数组 nums 和两个整数 k 和 f，判断是否存在一个长度至多为 k 的连续子数组，其中某个元素至少出现 f 次。
//
//示例 1：
//
//输入：nums = \[1, 2, 2, 3, 2, 4]，k = 4，f = 3
//
//输出：true
//
//解释：连续子数组 \[2, 2, 3, 2]（索引 1 - 4，长度 4）包含元素 2，其出现次数为 3 次。
//
//示例 2：
//
//输入：nums = \[1, 1, 1, 2, 2, 2]，k = 2，f = 3
//
//输出：false
//
//解释：在任何长度 <= 2 的连续子数组中，任何元素的最大出现次数都是 2。
//
//提示：
//
//1 <= nums.length <= 10 ^ 4
//
//- 10 ^ 9 <= nums \[i] <= 10 ^ 9
//
//1 <= k <= nums.length
//
//2 <= f <= k

#include <iostream>
#include<vector>
#include <unordered_map>
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

bool isMoreF(vector<long long>& arr, int k, int f)//k=length of window, f=frequency
{
	unordered_map<long long, int> window;
	int left = 0;
	int right = 0;
	int len = arr.size();
	for (; right < len; ++right)
	{
		window[arr[right]]++;
		if (right - left+1 >k)
		{
			window[arr[left]]--;
			if (window[arr[left]] == 0)
				window.erase(arr[left]);
			left++;
		}
		if (window[arr[right]] >= f)
			return true;
	}
	return false;
}
int main()
{
	long long k, f;
	vector<long long> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input k,f:\n";
	inputNum(k);
	inputNum(f);
	if (isMoreF(arr, k, f))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
