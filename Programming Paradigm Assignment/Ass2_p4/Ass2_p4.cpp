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
			cin.ignore(1000,'\n');
			cout << "Please re-enter a positive integer (2-20):\n";
		}
		else
			break;
	}
}
bool checkString(string& k, int p)//bit=p,true->right
{
	for (char c : k) {
		int val;
		if (isdigit(c)) {
			val = c - '0';
		}
		else if (isupper(c) && c <= 'J') { 
			val = c - 'A' + 10;
		}
		else {
			return false; 
		}
		if (val >= p) { 
			return false;
		}
	}
	return true;
}
void inputString(string& k,int bit)
{
	while (1)
	{
		cin >> k;
		if (!cin.good() || !checkString(k,bit))
		{
			cout << "Please re-enter a string:\n";
			cin.ignore( 1000,'\n' );
		}
		else
			break;
	}
}
//function for mirror
int charToInt(char c) {
	if (isdigit(c)) {
		return c - '0';
	}
	else {
		return c - 'A' + 10;
	}
}
char intToChar(int val) {
	if (val < 10) {
		return '0' + val;
	}
	else {
		return 'A' + val - 10;
	}
}
unsigned long long pToDecimal(const string& k, int p) {
	unsigned long long sum = 0; 
	unsigned long long exp = 1; 
	for (int i = k.size() - 1; i >= 0; --i) {
		int val = charToInt(k[i]);
		sum += val * exp;
		exp *= p;
	}
	return sum;
}
string decimalToM(unsigned long long decimal, int m) {
	if (decimal == 0) {
		return "0";
	}
	string mStr;
	while (decimal > 0) {
		int remainder = decimal % m;
		mStr += intToChar(remainder);
		decimal /= m;
	}
	return mStr;
}
void removeLeadingZeros(string& s) {
	int start = 0;
	// 找到第一个非零字符
	while (start < s.size() && s[start] == '0') {
		start++;
	}
	if (start == s.size()) { // 全是零，保留一个0
		s = "0";
	}
	else {
		s = s.substr(start);//从 start 位置开始，提取从 start 到字符串末尾的所有字符，并赋值给 s。
	}
}
string mirror(string&k,int bitPre,int bitAft)
{
	unsigned long long sum = pToDecimal(k, bitPre);
	string numBaseM = decimalToM(sum, bitAft);
	reverse(numBaseM.begin(), numBaseM.end());
	removeLeadingZeros(numBaseM);
	return numBaseM;
}
int main()//DO NOT support '-'minus
{
	int p, m;
	string k;
	cout << "Please input p,m and a string\n";
	inputNum(p);
	inputNum(m);
	inputString(k,p);
	cout << "The mirrored string is\n";
	cout << mirror(k, p, m);
	
	return 0;
}
