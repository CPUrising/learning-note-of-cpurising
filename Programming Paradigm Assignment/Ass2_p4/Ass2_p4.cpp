//2458620 古贺 终于后悔反省了
#include <iostream>
#include<string>
#include <algorithm>
using namespace std;
//function for input
void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 2 || len > 20)
		{
			cin.ignore('\n', 1000);
			cout << "Please re-enter a positive integer (2-20):\n";
		}
		else
			break;
	}
}
bool checkString(string& k, int bit)//bit=p,true->right
{
	char numEdge = '0' + 9;
	char charRight= numEdge;
	char charLeft= '0';
	if (bit <= 10)
		numEdge = '0' + bit - 1;
	else
	{
		charRight = 'A' + bit - 11;
		charLeft = 'A';
	}
	for (char c : k)
	{
		if (c >= '0' && c <= numEdge || c >= charLeft && c <= charRight);
		else
			return 0;
	}
	return 1;
}
void inputString(string& k,int bit)
{
	while (1)
	{
		cin >> k;
		if (!cin.good() || !checkString(k,bit))
		{
			cin.ignore('\n', 1000);
			cout << "Please re-enter a string:\n";
		}
		else
			break;
	}
}
//function for mirror
char intToChar(int num, int bit)
{
	string temp;
	if (num < 10)
		return '0' + num;
	else
	{
		return 'A' + bit - 11;
	}
}
unsigned long long pToDecimal(string& k, int bitPre)
{
	unsigned long long sum;
	for (int i = k.size() - 1, int exp = 1; i >= 0; i--)
	{
		sum += k[i] * exp;
		exp *= bitPre;
	}
	return sum;
}
string decimalToM(unsigned long long sum, int bitAft)
{
	string numBaseM;
	while (sum >0)
	{
		int numBit = sum % bitAft;
		numBaseM += intToChar(numBit, bitAft);
		sum /= bitAft;
	}
	return numBaseM;
}
void deleteZero(string& numBaseM)
{
	for (int i = numBaseM.size() - 1; numBaseM[i] == '0'; i--)
	{
		numBaseM.pop_back();
	}
}
string mirror(string&k,int bitPre,int bitAft)
{
	unsigned long long sum = pToDecimal(k, bitPre);
	string numBaseM = decimalToM(sum, bitAft);
	deleteZero(numBaseM);
	reverse(numBaseM.begin(), numBaseM.end());
	return numBaseM;
}
int main()//Don't forget to support '-'minus
{
	int p, m;
	string k;
	cout << "Please input p,m and a string\n";
	inputNum(p);
	inputNum(m);
	inputString(k,p);
	cout << "mirror后的string is\n";
	cout << mirror(k, p, m);
	
	return 0;
}
