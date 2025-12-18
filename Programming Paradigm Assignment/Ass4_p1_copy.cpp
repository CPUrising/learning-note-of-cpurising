//2458620 ??? ???????????
//题目 1
//
//设计一个名为`BankAccount`的类，用于模拟简单的银行系统。每个账户需包含以下信息：
//
//* 账户名（默认值："anonymous"）
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

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class BankAccount
{
private:
    string accountName;
    double balance;//???????????0???
    string accountType;
public:
    BankAccount(const string& name = "anonymous", double bal = 0.0, const string& type = "???????") :
        accountName(name), balance(bal), accountType(type) {
    }
    void deposit(double amount);
    void withdraw(double amount);
    bool operator==(const BankAccount& other)const;
    bool operator!=(const BankAccount& other)const;
    void display() const;
    // 添加设置账户信息的方法，用于用户输入
    void setAccountInfo(const string& name, double bal, const string& type) {
        accountName = name;
        balance = bal;
        accountType = type;
    }
};

int main() {
    // 用户输入账户1信息
    BankAccount acc1;
    string name1;
    double bal1;
    string type1;
    cout << "请输入账户1信息：" << endl;
    cout << "账户名称：";
    cin >> name1;
    cout << "账户余额：";
    cin >> bal1;
    cout << "账户类型：";
    cin >> type1;
    acc1.setAccountInfo(name1, bal1, type1);
    cout << "\n账户1信息：" << endl;
    acc1.display();

    // 用户输入账户2信息
    BankAccount acc2;
    string name2;
    double bal2;
    string type2;
    cout << "\n请输入账户2信息：" << endl;
    cout << "账户名称：";
    cin >> name2;
    cout << "账户余额：";
    cin >> bal2;
    cout << "账户类型：";
    cin >> type2;
    acc2.setAccountInfo(name2, bal2, type2);
    cout << "\n账户2信息：" << endl;
    acc2.display();

    // 用户输入存款金额
    double depositAmount;
    cout << "\n请输入账户1的存款金额：";
    cin >> depositAmount;
    acc1.deposit(depositAmount);
    cout << "账户1存款后信息：" << endl;
    acc1.display();

    // 用户输入取款金额
    double withdrawAmount;
    cout << "\n请输入账户2的取款金额：";
    cin >> withdrawAmount;
    acc2.withdraw(withdrawAmount);
    cout << "账户2取款后信息：" << endl;
    acc2.display();

    // 再次取款（用户输入）
    double anotherWithdraw;
    cout << "\n请再次输入账户2的取款金额：";
    cin >> anotherWithdraw;
    acc2.withdraw(anotherWithdraw);

    // 用户输入账户3和4信息
    BankAccount acc3, acc4;
    string name3, name4;
    cout << "\n请输入账户3名称：";
    cin >> name3;
    acc3.setAccountInfo(name3, 0.0, "储蓄账户");  // 余额和类型使用默认值
    cout << "请输入账户4名称：";
    cin >> name4;
    acc4.setAccountInfo(name4, 0.0, "储蓄账户");  // 余额和类型使用默认值

    cout << "\n账户2和账户3是否相等：" << (acc2 == acc3 ? "是" : "否") << endl;
    cout << "账户2和账户4是否相等：" << (acc2 == acc4 ? "是" : "否") << endl;
    cout << "账户2和账户4是否不等：" << (acc2 != acc4 ? "是" : "否") << endl;

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
    cout << "账户名称：" << accountName << endl;
    cout << "账户余额：" << fixed << setprecision(2) << balance << " 元" << endl;
    cout << "账户类型：" << accountType << endl;
}

// 重载相等判断：比较账户名称
bool BankAccount::operator==(const BankAccount& other) const {
    return accountName == other.accountName;
}

// 重载不等判断：比较账户名称
bool BankAccount::operator!=(const BankAccount& other) const {
    return !(*this == other);
}
