void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good() || len < 2 || len > 20)
		{
			cin.clear();
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
			cin.clear();
			cin.ignore( 1000,'\n' );
		}
		else
			break;
	}
}
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
bool isNum(string& ch) {
    // 空字符串不是数字
    if (ch.empty()) return false;
    // 第一个字符是 '-' 且长度 >1，后面必须全是数字
    if (ch[0] == '-' && ch.size() > 1) {
        for (int i = 1; i < ch.size(); i++) {
            if (!isdigit(ch[i])) return false;
        }
        return true;
    }
    // 全部字符都是数字
    for (char c : ch) {
        if (!isdigit(c)) return false;
    }
    return true;
}
int stringToInt(string& s) {
    return stoi(s); // 使用标准库函数，简洁且正确
}
