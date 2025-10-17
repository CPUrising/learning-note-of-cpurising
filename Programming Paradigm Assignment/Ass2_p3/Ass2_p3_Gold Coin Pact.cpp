//2458620 古贺 终于后悔反省了
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
			cin.ignore('\n', 1000);
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
	vector<int>::iterator it = arr.end() - 1;
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
