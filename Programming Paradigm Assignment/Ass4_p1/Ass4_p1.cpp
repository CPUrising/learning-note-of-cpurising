//2458620 ??? ???????????
//题目 1
//
//设计一个名为`BankAccount`的类，用于模拟简单的银行系统。每个账户需包含以下信息：
//
//* 账户名（默认值："anonymous"9）
//
//* 账户余额（默认值：0.0）
//
//* 账户类型（"储蓄账户" 或 "活期账户"，默认值："储蓄账户"）
//
//
//该类需支持以下操作：
//
//1. `deposit(amount)`—— 将指定金额存入账户余额。
//
//2. `withdraw(amount)`—— 从账户余额中提取指定金额；若余额不足，显示警告信息。
//
//3. `display()`—— 打印或返回所有账户详情（账户名、余额、账户类型）。
//
//4. 相等性判断（`= = `）—— 若两个账户属于同一账户名，返回`True`。
//
//5. 不等性判断（`! = `）—— 若两个账户属于不同账户名，返回`True`。
//
//
//允许同时存在多个 "anonymous" 账户，但它们的余额和交易记录必须完全独立。
//2458620 古贺 终于后悔反省了
#include <iostream>
#include <string>
using namespace std;

class BankAccount
{
public:
	BankAccount(string accountName = "anonymous", double accountBalance = 0.0, string accountType = "储蓄账户") :
		accountName(accountName), accountBalance(accountBalance), accountType(accountType) {};
	~BankAccount() {};
	void deposit(double amount);
	void withdraw(double amount);
	void display();
	bool operator==(const BankAccount& other)const;
	bool operator!=(const BankAccount& other)const;
private:
	string accountName;
	double accountBalance;//余额
	string accountType;
};

void BankAccount::deposit(double amount)
{
	accountBalance += amount;
	cout << "存入" << amount << endl;
	cout << "剩余" << accountBalance << endl;
}

void BankAccount::withdraw(double amount)
{
	if(accountBalance>=amount)
	{
		accountBalance -= amount;
		cout << "取出" << amount << endl;
		cout << "剩余" << accountBalance << endl;
	}
}

void BankAccount::display()
{
	cout << "账户名" << accountName << endl;
	cout << "账户类型" << accountType << endl;
	cout << "存款" << accountBalance << endl;
}

bool BankAccount::operator==(const BankAccount& other)const
{
	return accountName == other.accountName;
}

bool BankAccount::operator!=(const BankAccount& other)const
{
	return accountName != other.accountName;
}

int main()
{
	BankAccount acc1;
	acc1.display();
	acc1.deposit(1000.5);
	acc1.withdraw(500.2);
	acc1.withdraw(600);  // 余额不足测试

	// 测试2：自定义账户
	BankAccount acc2("张三", 2000, "活期账户");
	acc2.display();

	// 测试3：相等性判断
	BankAccount acc3("张三");
	if (acc2 == acc3)
	{
		cout << "acc2 和 acc3 账户名相同" << endl;
	}
	if (acc1 != acc2)
	{
		cout << "acc1 和 acc2 账户名不同" << endl;
	}

	return 0;
}
