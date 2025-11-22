#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

// 家族成员类（多叉树节点）
class FamilyMember {
public:
    string name;                  // 成员姓名
    vector<FamilyMember*> children; // 子女列表（子节点）

    FamilyMember(const string& n) : name(n) {}
};

// 家谱管理类（无map依赖）
class Genealogy {
private:
    FamilyMember* root;                  // 家谱根节点（祖先）
    vector<FamilyMember*> allMembers;    // 存储所有成员（替代map，用于查找）

    // 线性查找成员（返回节点指针，找不到返回nullptr）
    FamilyMember* findMember(const string& name) {
        for (FamilyMember* member : allMembers) {
            if (member->name == name) {
                return member;
            }
        }
        return nullptr;
    }

public:
    Genealogy() : root(nullptr) {}

    // 1. 初始化家谱（建立祖先）
    void init(const string& ancestorName) {
        root = new FamilyMember(ancestorName);
        allMembers.push_back(root);  // 添加到全局列表
        cout << "此家谱的祖先是：" << ancestorName << endl;
    }

    // 2. 完善家庭（为成员添加多个子女）
    bool completeFamily(const string& parentName, const vector<string>& childNames) {
        FamilyMember* parent = findMember(parentName);
        if (parent == nullptr) {
            cout << "错误：未找到成员" << parentName << endl;
            return false;
        }
        for (const string& childName : childNames) {
            // 检查是否已存在（线性查找）
            if (findMember(childName) != nullptr) {
                cout << "警告：成员" << childName << "已存在" << endl;
                continue;
            }
            FamilyMember* child = new FamilyMember(childName);
            parent->children.push_back(child);
            allMembers.push_back(child);  // 添加到全局列表
        }
        // 输出结果
        cout << parentName << "的第一子代子孙是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name;
        }
        cout << endl;
        return true;
    }

    // 3. 添加家庭成员（为成员添加单个子女）
    bool addMember(const string& parentName, const string& childName) {
        FamilyMember* parent = findMember(parentName);
        if (parent == nullptr) {
            cout << "错误：未找到成员" << parentName << endl;
            return false;
        }
        if (findMember(childName) != nullptr) {
            cout << "警告：成员" << childName << "已存在" << endl;
            return false;
        }
        FamilyMember* child = new FamilyMember(childName);
        parent->children.push_back(child);
        allMembers.push_back(child);  // 添加到全局列表
        cout << parentName << "的第一子代子孙是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name;
        }
        cout << endl;
        return true;
    }

    // 4. 解散家庭（删除成员的所有子女）
    bool dissolveFamily(const string& parentName) {
        FamilyMember* parent = findMember(parentName);
        if (parent == nullptr) {
            cout << "错误：未找到成员" << parentName << endl;
            return false;
        }
        // 输出要解散的子女
        cout << parentName << "要解散家庭的人是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name;
            // 从全局列表中移除
            for (auto it = allMembers.begin(); it != allMembers.end(); ++it) {
                if (*it == parent->children[i]) {
                    allMembers.erase(it);
                    break;
                }
            }
            // 释放内存
            delete parent->children[i];
        }
        cout << endl;
        parent->children.clear();
        return true;
    }

    // 5. 更改成员姓名
    bool renameMember(const string& oldName, const string& newName) {
        FamilyMember* member = findMember(oldName);
        if (member == nullptr) {
            cout << "错误：未找到成员" << oldName << endl;
            return false;
        }
        // 检查新姓名是否已存在
        if (findMember(newName) != nullptr) {
            cout << "警告：姓名" << newName << "已被使用" << endl;
            return false;
        }
        member->name = newName;
        cout << oldName << "已更名为" << newName << endl;
        return true;
    }

    // 新增：检查成员是否存在（给输入检查函数调用）
    bool isMemberExists(const string& name) {
        return findMember(name) != nullptr;
    }

    // 析构函数：释放所有节点内存
    ~Genealogy() {
        for (FamilyMember* member : allMembers) {
            delete member;
        }
        allMembers.clear();
        root = nullptr;
    }
};

// 清除输入缓冲区
void clearInputBuffer() {
    cin.clear();
    cin.ignore(1000, '\n');
}

// 输入检查：正整数（1-20，用于儿女人数）
void inputNum(int& len) {
    while (1) {
        cin >> len;
        if (!cin.good() || len < 1 || len > 20) {
            clearInputBuffer();
            cout << "输入错误！请重新输入正整数（1-20）：";
        }
        else {
            clearInputBuffer();
            break;
        }
    }
}

// 修改：仅检查姓名非空
bool checkNameFormat(const string& name) {
    return !name.empty(); // 只要非空字符串即可
}

// 输入检查：姓名格式（仅检查非空）
void inputName(string& name) {
    while (1) {
        // 使用getline读取整行，允许包含空格
        getline(cin, name);
        if (!cin.good() || !checkNameFormat(name)) {
            clearInputBuffer();
            cout << "姓名不能为空，请重新输入：";
        }
        else {
            break;
        }
    }
}

// 输入检查：已存在的成员姓名（用于父成员、要改名的成员）
void inputExistingName(string& name, Genealogy& genealogy) {
    while (1) {
        inputName(name);
        if (genealogy.isMemberExists(name)) {
            break;
        }
        else {
            cout << "错误：该成员不存在，请重新输入：";
        }
    }
}

// 输入检查：不存在的成员姓名（用于新增子女、改名后的姓名）
void inputNewName(string& name, Genealogy& genealogy) {
    while (1) {
        inputName(name);
        if (!genealogy.isMemberExists(name)) {
            break;
        }
        else {
            cout << "错误：该姓名已存在，请重新输入：";
        }
    }
}

// 输入检查：操作选项（A-E）
void inputChoice(char& choice) {
    while (1) {
        cin >> choice;
        choice = toupper(choice);
        if (!cin.good() || !(choice >= 'A' && choice <= 'E')) {
            clearInputBuffer();
            cout << "输入错误！请选择A-E中的操作：";
        }
        else {
            clearInputBuffer();
            break;
        }
    }
}
// -------------------------- 输入检查函数结束 --------------------------

// 主函数：交互界面
int main() {
    cout << "** 家谱管理系统 **" << endl;
    cout << "** 请选择要执行的操作： **" << endl;
    cout << "** A ---- 完善家谱 **" << endl;
    cout << "** B ---- 添加家庭成员 **" << endl;
    cout << "** C ---- 解散局部家庭 **" << endl;
    cout << "** D ---- 更改家庭成员姓名 **" << endl;
    cout << "** E ---- 退出程序 **" << endl;
    cout << "************************" << endl;

    Genealogy genealogy;
    string ancestorName;
    cout << "首先建立一个家谱！" << endl;
    cout << "请输入祖先的姓名：";
   
    inputNewName(ancestorName, genealogy); // 祖先姓名需是新的（不存在）
    genealogy.init(ancestorName);

    char choice;
    while (true) {
        cout << "\n请选择要执行的操作：";
        inputChoice(choice); // 检查操作选项合法性

        if (choice == 'A') {
            string parentName;
            int childCount;
            cout << "请输入要建立家庭的人的姓名：";
            inputExistingName(parentName, genealogy); // 父成员必须存在
            cout << "请输入" << parentName << "的儿女人数：";
            inputNum(childCount); // 检查儿女人数合法性
            cout << "请依次输入" << parentName << "的儿女的姓名：";
            vector<string> childNames(childCount);
            for (int i = 0; i < childCount; ++i) {
                inputNewName(childNames[i], genealogy); // 子女姓名必须不存在
            }
            genealogy.completeFamily(parentName, childNames);
        }
        else if (choice == 'B') {
            string parentName, childName;
            cout << "请输入要添加儿子（或女儿）的人的姓名：";
            inputExistingName(parentName, genealogy); // 父成员必须存在
            cout << "请输入" << parentName << "新添加的儿子（或女儿）的姓名：";
            inputNewName(childName, genealogy); // 子女姓名必须不存在
            genealogy.addMember(parentName, childName);
        }
        else if (choice == 'C') {
            string parentName;
            cout << "请输入要解散家庭的人的姓名：";
            inputExistingName(parentName, genealogy); // 父成员必须存在
            genealogy.dissolveFamily(parentName);
        }
        else if (choice == 'D') {
            string oldName, newName;
            cout << "请输入要更改姓名的人的目前姓名：";
            inputExistingName(oldName, genealogy); // 原成员必须存在
            cout << "请输入更改后的姓名：";
            inputNewName(newName, genealogy); // 新姓名必须不存在
            genealogy.renameMember(oldName, newName);
        }
        else if (choice == 'E') {
            cout << "程序已退出" << endl;
            break;
        }
    }

    return 0;
}