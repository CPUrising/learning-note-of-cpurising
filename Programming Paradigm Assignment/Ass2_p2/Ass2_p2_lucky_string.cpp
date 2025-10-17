//2458620 古贺 终于后悔反省了
#include <iostream>
using namespace std;

void input(int &len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 1 || len > 10)
		{
			cin.ignore('\n', 1000);
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
