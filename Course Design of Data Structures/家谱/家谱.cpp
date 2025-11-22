#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <cctype>

using namespace std;

// 家族成员类（多叉树节点，添加唯一ID）
class FamilyMember {
public:
    int id;                       // 唯一标识ID
    string name;                  // 成员姓名（允许重复）
    vector<FamilyMember*> children; // 子女列表

    FamilyMember(int id, const string& n) : id(id), name(n) {}
};

// 家谱管理类（支持重复姓名）
class Genealogy {
private:
    FamilyMember* root;                  // 家谱根节点
    vector<FamilyMember*> allMembers;    // 存储所有成员
    int nextId = 1;                      // ID计数器（确保唯一性）

    // 查找所有同名成员
    vector<FamilyMember*> findAllMembers(const string& name) {
        vector<FamilyMember*> result;
        for (FamilyMember* member : allMembers) {
            if (member->name == name) {
                result.push_back(member);
            }
        }
        return result;
    }

    // 通过ID查找成员
    FamilyMember* findMemberById(int id) {
        for (FamilyMember* member : allMembers) {
            if (member->id == id) {
                return member;
            }
        }
        return nullptr;
    }

public:
    Genealogy() : root(nullptr) {}

    // 初始化家谱（建立祖先）
    void init(const string& ancestorName) {
        root = new FamilyMember(nextId++, ancestorName);
        allMembers.push_back(root);
        cout << "此家谱的祖先是：" << ancestorName << "（ID：" << root->id << "）" << endl;
    }

    // 完善家庭（添加多个子女）
    bool completeFamily(int parentId, const vector<string>& childNames) {
        FamilyMember* parent = findMemberById(parentId);
        if (parent == nullptr) {
            cout << "错误：未找到ID对应的成员" << endl;
            return false;
        }
        for (const string& childName : childNames) {
            FamilyMember* child = new FamilyMember(nextId++, childName);
            parent->children.push_back(child);
            allMembers.push_back(child);
        }
        // 输出结果（包含ID）
        cout << parent->name << "（ID：" << parent->id << "）的第一子代子孙是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name << "（ID：" << parent->children[i]->id << "）";
        }
        cout << endl;
        return true;
    }

    // 添加单个家庭成员
    bool addMember(int parentId, const string& childName) {
        FamilyMember* parent = findMemberById(parentId);
        if (parent == nullptr) {
            cout << "错误：未找到ID对应的成员" << endl;
            return false;
        }
        FamilyMember* child = new FamilyMember(nextId++, childName);
        parent->children.push_back(child);
        allMembers.push_back(child);
        cout << parent->name << "（ID：" << parent->id << "）的第一子代子孙是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name << "（ID：" << parent->children[i]->id << "）";
        }
        cout << endl;
        return true;
    }

    // 解散家庭（删除所有子女）
    bool dissolveFamily(int parentId) {
        FamilyMember* parent = findMemberById(parentId);
        if (parent == nullptr) {
            cout << "错误：未找到ID对应的成员" << endl;
            return false;
        }
        cout << parent->name << "（ID：" << parent->id << "）要解散家庭的人是：";
        for (size_t i = 0; i < parent->children.size(); ++i) {
            if (i > 0) cout << " ";
            cout << parent->children[i]->name << "（ID：" << parent->children[i]->id << "）";
            // 从全局列表移除
            for (auto it = allMembers.begin(); it != allMembers.end(); ++it) {
                if (*it == parent->children[i]) {
                    allMembers.erase(it);
                    break;
                }
            }
            delete parent->children[i];
        }
        cout << endl;
        parent->children.clear();
        return true;
    }

    // 更改成员姓名（无需检查重名）
    bool renameMember(int memberId, const string& newName) {
        FamilyMember* member = findMemberById(memberId);
        if (member == nullptr) {
            cout << "错误：未找到ID对应的成员" << endl;
            return false;
        }
        string oldName = member->name;
        member->name = newName;
        cout << oldName << "（ID：" << member->id << "）已更名为" << newName << endl;
        return true;
    }

    // 获取同名成员列表（供输入检查使用）
    vector<FamilyMember*> getMembersByName(const string& name) {
        return findAllMembers(name);
    }

    // 析构函数
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

// 输入检查：正整数（1-20）
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

// 输入检查：ID（正整数）
void inputId(int& id) {
    while (1) {
        cin >> id;
        if (!cin.good() || id < 1) {
            clearInputBuffer();
            cout << "输入错误！请重新输入正整数ID：";
        }
        else {
            clearInputBuffer();
            break;
        }
    }
}

// 检查姓名非空
bool checkNameFormat(const string& name) {
    return !name.empty();
}

// 输入姓名（允许重复）
void inputName(string& name) {
    while (1) {
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

// 输入成员姓名并选择具体ID（处理重名）
int inputMemberId(Genealogy& genealogy) {
    string name;
    while (1) {
        cout << "请输入成员姓名：";
        inputName(name);
        vector<FamilyMember*> members = genealogy.getMembersByName(name);
        if (members.empty()) {
            cout << "错误：该姓名的成员不存在，请重新输入" << endl;
            continue;
        }
        // 显示所有同名成员
        cout << "找到以下同名成员，请选择ID：" << endl;
        for (FamilyMember* m : members) {
            cout << m->name << "（ID：" << m->id << "）" << endl;
        }
        // 输入并验证ID
        int id;
        cout << "请输入目标成员的ID：";
        inputId(id);
        for (FamilyMember* m : members) {
            if (m->id == id) {
                return id; // 找到匹配的ID
            }
        }
        cout << "错误：输入的ID与该姓名不匹配，请重新选择" << endl;
    }
}

// 输入操作选项（A-E）
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

// 主函数
int main() {
    cout << "** 家谱管理系统（支持重复姓名）**" << endl;
    cout << "** 操作说明：所有成员通过唯一ID标识，姓名可重复 **" << endl;
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
    inputName(ancestorName); // 祖先姓名无需检查唯一性
    genealogy.init(ancestorName);

    char choice;
    while (true) {
        cout << "\n请选择要执行的操作：";
        inputChoice(choice);

        if (choice == 'A') {
            int parentId = inputMemberId(genealogy);
            int childCount;
            cout << "请输入该成员的儿女人数：";
            inputNum(childCount);
            cout << "请依次输入儿女的姓名（可重复）：";
            vector<string> childNames(childCount);
            for (int i = 0; i < childCount; ++i) {
                inputName(childNames[i]); // 无需检查重名
            }
            genealogy.completeFamily(parentId, childNames);
        }
        else if (choice == 'B') {
            int parentId = inputMemberId(genealogy);
            string childName;
            cout << "请输入新添加的儿子（或女儿）的姓名：";
            inputName(childName); // 无需检查重名
            genealogy.addMember(parentId, childName);
        }
        else if (choice == 'C') {
            int parentId = inputMemberId(genealogy);
            genealogy.dissolveFamily(parentId);
        }
        else if (choice == 'D') {
            int memberId = inputMemberId(genealogy);
            string newName;
            cout << "请输入更改后的姓名：";
            inputName(newName); // 无需检查重名
            genealogy.renameMember(memberId, newName);
        }
        else if (choice == 'E') {
            cout << "程序已退出" << endl;
            break;
        }
    }

    return 0;
}