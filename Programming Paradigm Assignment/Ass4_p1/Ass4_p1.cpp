//2458620 古贺 终于后悔反省了
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class BankAccount
{
private:
	string accountName;   
	double balance;//怎么做到大于0排除
	string accountType;   
public:
	BankAccount(const string& name = "anonymous", double bal=0.0,const string& type="储蓄账户") :
		accountName(name), balance(bal), accountType(type){ }
	void deposit(double amount);
	void withdraw(double amount);
	bool operator==(const BankAccount& other)const;
	bool operator!=(const BankAccount& other)const;
	void display() const;
};
int main() {
    BankAccount acc1;
    cout << "账户1详情：" << endl;
    acc1.display();

    BankAccount acc2("张三", 1000.0, "活期账户");
    cout << "\n账户2详情：" << endl;
    acc2.display();

    acc1.deposit(500);
    cout << "\n账户1存款后详情：" << endl;
    acc1.display();

    acc2.withdraw(300);
    cout << "\n账户2取款后详情：" << endl;
    acc2.display();

    acc2.withdraw(1000);

    BankAccount acc3("张三");
    BankAccount acc4("李四");
    cout << "\n账户2与账户3是否相等：" << (acc2 == acc3 ? "是" : "否") << endl;
    cout << "账户2与账户4是否相等：" << (acc2 == acc4 ? "是" : "否") << endl;
    cout << "账户2与账户4是否不等：" << (acc2 != acc4 ? "是" : "否") << endl;

    return 0;
}
void BankAccount::deposit(double amount)
{
	balance += amount;
}
void BankAccount::withdraw(double amount) {
	if (amount <= 0) return;

	if (amount <= balance) {
		balance -= amount;
	}
	else {
		cout << "警告：余额不足，无法提取 " << amount << " 元" << endl;
	}
}
void BankAccount::display() const {
	cout << "账户名：" << accountName << endl;
	cout << "账户余额：" << fixed << setprecision(2) << balance << " 元" << endl;
	cout << "账户类型：" << accountType << endl;
}

// 相等性判断（基于账户名）
bool BankAccount::operator==(const BankAccount& other) const {
	return accountName == other.accountName;
}

// 不等性判断（基于账户名）
bool BankAccount::operator!=(const BankAccount& other) const {
	return !(*this == other);
}