//2458620 古贺 终于后悔反省了
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
//ctrl,click function and jump,ladies and gentlemen.
void removeLeadingZeros(string& numA);
void multiple(string&numA,string&numB, string& numC);
void inputString(string& s);
bool checkString(string& k);

int main()
{
	string numA;
	string numB;
	string numC;
	cout << "Please input two unsigned integers\n";
	inputString(numA);
	inputString(numB);
	multiple(numA, numB, numC);
	cout << "The result of the product is \n"<< numC<<endl;
	return 0;
}

bool checkString(string& k)
{
	for (char c : k) 
		if (!isdigit(c))
			return false;
	return true;
}
void inputString(string& k)
{
	while (1)
	{
		cin >> k;
		if (!cin.good() || !checkString(k))
		{
			cout << "Please re-enter a num:\n";
			cin.ignore(1000, '\n');
		}
		else
			break;
	}
}
void removeLeadingZeros( string& s) {
	int start = 0;
	while (start < s.size() && s[start] == '0') {
		start++;
	}
	if (start == s.size()) { // 全是零
		s = "0";
	}
	else
		s = s.substr(start);
}
void multiple(string& numA, string& numB, string& productFinal)
{
	removeLeadingZeros(numA);
	removeLeadingZeros(numB);
	if (numA == "0" || numB == "0")
	{
		productFinal = "0";
		return;
	}
	int aSize = numA.size();
	int bSize = numB.size();
	vector<int>result(aSize + bSize, 0);
	for (int i = aSize - 1; i >= 0; i--)
	{
		int digitA = numA[i] - '0';
		for (int j = bSize - 1; j >= 0; j--)
		{
			int digitB = numB[j] - '0';
			int product = digitA * digitB;
			int sum = product + result[i + j + 1];
			result[i + j + 1] = sum % 10; 
			result[i + j] += sum / 10;
		}
	}
	for (int num : result) {
		// 跳过开头的0（除非结果就是0，但前面已处理）
		if (!(productFinal.empty() && num == 0)) {
			productFinal += (char)(num + '0');
		}
	}
}