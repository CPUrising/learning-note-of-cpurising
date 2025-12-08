//2458620 古贺 终于后悔反省了
//问题 3：金币约定
//
//毛有一块长度为 n 的巧克力，他想把它掰成若干 * *长度互不相同 * *的小块，且要让所有小块长度的 * *乘积尽可能大 * *—— 乘积越大，他能获得的收益越多！
//
//请帮毛找到最赚钱的巧克力拆分方式。
//
//* *输入 * *：一个整数 n（3 ≤ n ≤ 10000）
//
//* *输出 * *：输出一种将 n 拆分为若干不同正整数的方案（要求乘积最大），数字之间用单个空格分隔，且按升序排列。
//
//* *示例输入 * *：10
//
//* *示例输出 * *：2 3 5
#include <iostream>
#include<vector>
using namespace std;

void input(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 3 || len > 10000)
		{
			cin.ignore(1000,'\n');
			cout << "Please re-enter a positive integer (3-10000):\n";
		}
		else
			break;
	}
}
void arrayLargest(int len, vector<int>&arr)
{
	int sum = 0;
	int current = 2;
	while (sum + current <=len) {
		arr.push_back(current);
		sum += current;
		current++;
	}
	int d = len-sum;
	if (d == 0) return;
	vector<int>::iterator it = arr.end() - 1;//carefull
	while (d-- > 0 && it>=arr.begin())
	{
		++*it;
		if (it != arr.begin()) {
			it--;
		}
	}
	if (d > 0)
		arr.back() += d;
}
int main()
{
	int len;
	vector<int> arr;
	cout << "Input the length of the chocolate.\n";
	input(len);
	arrayLargest(len, arr);
	cout << "The optimal decomposition is:\n";
	for (int n : arr)
		cout << n << ' ';
	return 0;
}
