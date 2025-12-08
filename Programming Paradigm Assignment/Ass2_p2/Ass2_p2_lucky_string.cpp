//2458620 古贺 终于后悔反省了
//问题 2：毛的幸运字符串
//
//有一种字符串被称为 “毛的幸运字符串”，其定义如下：
//
//* 字符串索引从 0 开始。
//
//* 位于 * *偶数索引 * *（0、2、4……）的字符必须是元音字母（a、e、i、o、u）。
//
//* 位于 * *奇数索引 * *（1、3、5……）的字符必须是辅音字母（其余 21 个小写英文字母）。
//
//
//* *示例 * *：“abi” 是幸运字符串（a 位于索引 0，b 位于索引 1，i 位于索引 2）。
//
//字符串允许出现重复字符，且可由任意符合规则的字母开头。
//
//现在，毛把他的巧克力冰淇淋锁在了冰箱里，锁上显示一个数字 n（1 ≤ n ≤ 10），密码是 “长度为 n 的毛的幸运字符串” 的可能数量。请帮毛计算这个数量。
//
//* *输入 * *：一个整数 n
//
//* *输出格式 * *：一个整数 —— 可能的幸运字符串数量
//
//* *示例 * *
//
//输入：1
//
//输出：5
//
//解释：当 n = 1 时，字符串只能由元音字母组成，共 5 种可能：a、e、i、o、u。
#include <iostream>
using namespace std;

void input(int &len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 1 || len > 10)
		{
			cin.ignore(1000,'\n');
			cout << "Please re-enter a positive integer (1-10):\n";
		}
		else
			break;
	}
}
long long count_single(int& len, int& bit, int weight)//weight=5 or 21
{
	int single = 1;
	for (; bit <= len; bit += 2)
	{
		single *= weight;
	}
	return single;
}
long long count_lucky(int len)
{
	int odd=1, even=2;
	long long odd_sum = count_single(len, odd, 5);
	long long even_sum = count_single(len, even, 21);
	return odd_sum * even_sum;
}
int main()
{
	int len;
	cout << "Input the length of the lucky string.\n";
	input(len);
	long long num = count_lucky(len);
	cout << "The number of possible lucky strings is: " << num << endl;
	return 0;
}
