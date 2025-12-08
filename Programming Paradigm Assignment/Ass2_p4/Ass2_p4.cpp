//2458620 古贺 终于后悔反省了
//问题 4：镜像编码
//
//毛发现了一面神奇的 “数字镜子”。当他输入一个数字字符串时，镜子会按以下规则处理：
//
//1. 将该字符串视为 p 进制数；
//
//2. 把这个 p 进制数转换成 m 进制数；
//
//3. 将转换后的 m 进制数字符串 * *反转顺序 * *后显示。
//
//
//注意：反转结果中的前导零需去除（即从反转字符串中第一个非零数字开始输出）。
//
//* *输入 * *：一行包含三个元素，分别是整数 p、整数 m 和字符串 k，满足：
//
//* 2 ≤ p、m ≤ 20；
//
//* k 是有效的 p 进制数（数字使用 0 - 9 和 A - J 表示 10 - 19，均为大写）。
//
//
//* *输出 * *：输出镜像处理后的结果：将 k 从 p 进制转换为 m 进制，反转得到的 m 进制字符串，去除前导零后输出（数字 10 - 19 用 A - J 表示）。
//
//* *示例 * *
//
//输入：8 12 157
//
//输出：39
//
//解释：8 进制数 157 转换为十进制是 64 + 40 + 7 = 111；十进制数 111 转换为 12 进制是 “93”；反转后得到 “39”，输出 39。
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
