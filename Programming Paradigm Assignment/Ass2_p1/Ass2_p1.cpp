//2450721 古贺 终于后悔反省了
//问题 1：故事结尾回文改造
//
//假设你是某大型出版公司的编辑，需要修改一个故事的结尾，使其成为一个回文字符串。该结尾是由小写字母组成的字符串，但目前还不是回文。为实现完美效果，编辑团队希望通过 * *最少的修改次数 * *将其变为回文，且在存在多种最少修改方案时，需选择 * *字典序最小 * *的回文结果。
//
//（提示：对于长度相同的两个字符串 a 和 b，若在首个不同字符的位置上，a 中的字符在字母表中出现的顺序早于 b 中的字符，则认为 a 的字典序小于 b。）
//
//* *输入 * *：一个非空字符串
//
//* *输出 * *：返回经过最少修改得到的回文字符串；若有多个解，返回字典序最小的那个。
//
//* *示例 1 * *
//
//输入：“sunday”
//
//输出：“saddas”
//
//解释：“saddas” 是回文字符串，且修改次数最少、字典序最小。
//
//* *示例 2 * *
//
//输入：“seven”
//
//输出：“neven”
//
//解释：“neven” 是回文字符串，且修改次数最少、字典序最小。
#include <iostream>
#include <string>
using namespace std;

bool check(string& reverse)
{
	for (char c : reverse)
	{
		if (c < 'a' || c>'z')
			return false;
	}
	return true;
}
void edit_palindrome(string& reverse, string::iterator start, string::iterator end)
{
	while (start<end)
	{
		if (*start > *end)
		{
			*start = *end;
		}
		else if(*start < *end)
			*end = *start;
		start++;
		end--;
	}
}
int main()
{
	string reverse;
	while(true)
	{
		cout << "Input a string, which should contain only lowercase letters\n";
		getline(cin, reverse);
		if (reverse.empty())
		{
			cout << "Empty string entered, please re-enter\n";
		}
		if (!check(reverse))
		{
			cout << "Please re-enter, the string should contain only lowercase letters\n";
			continue;
		}
		else
			break;
	}
	cout << "Original string is:\n" << reverse << endl;
	string::iterator start = reverse.begin();
	string::iterator end = reverse.end()-1;//小心end位置
	edit_palindrome(reverse, start, end);
	cout << "The reversed one is:\n" << reverse << endl;

	return 0;
}
