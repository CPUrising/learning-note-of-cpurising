//2450721 古贺 终于后悔反省了
#include <iostream>
#include <vector>
using namespace std;
int main()
{
	vector <float> squars;
	for (int i = 0; i < 25; i++)
	{
		squars.push_back(i+1);
	}
	cout << "Before square\n";
	for (float n : squars)
	{
		cout << n << ' ';
	}
	cout << endl;
	for (float &n : squars)
	{
		n *= n;
	}
	cout << "After square\n";
	for (float n : squars)
	{
		cout << n << ' ';
	}

	return 0;
}
