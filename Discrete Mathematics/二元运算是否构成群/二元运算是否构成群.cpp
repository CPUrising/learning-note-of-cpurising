#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <algorithm>
using namespace std;

// 输入验证：确保输入为非空字符串（集合元素/运算结果）
string inputValidElement(const string& prompt) {
    string elem;
    while (true) {
        cout << prompt;
        getline(cin, elem);
        // 去除首尾空格（允许元素含中间空格，如"a+b"）
        elem.erase(0, elem.find_first_not_of(" \t\n\r"));
        elem.erase(elem.find_last_not_of(" \t\n\r") + 1);
        if (!elem.empty()) break;
        cout << "输入无效！元素不能为空，请重新输入。" << endl;
    }
    return elem;
}

// 主函数：判断集合关于二元运算是否构成群
int main() {
    cout << "===== 有限集合关于二元运算的群判定程序 =====" << endl;

    // 步骤1：输入有限集合G
    int n;
    cout << "\n步骤1：输入集合的元素个数（正整数）：";
    while (!(cin >> n) || n <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "输入无效！请输入正整数：";
    }
    cin.ignore(1000, '\n');  // 清除缓冲区换行符

    vector<string> setG;          // 存储集合元素（支持字符串类型，如"0","1","a","x+y"）
    unordered_set<string> setG_set; // 用于快速判断元素是否在集合中（O(1)查询）
    cout << "步骤2：输入集合的" << n << "个元素（每个元素占一行，不可重复）：" << endl;
    for (int i = 0; i < n; ++i) {
        string elem = inputValidElement("元素" + to_string(i + 1) + "：");
        if (setG_set.count(elem)) {
            cout << "元素重复！请重新输入不重复的元素。" << endl;
            i--;  // 重新输入当前元素
            continue;
        }
        setG.push_back(elem);
        setG_set.insert(elem);
    }

    // 步骤3：输入二元运算表（用map存储：key=(a,b)，value=a*b）
    cout << "\n步骤3：输入二元运算表（运算记为*，格式：a b a*b，每行一个运算结果）" << endl;
    cout << "提示：需输入所有" << n * n << "种元素对的运算结果（元素顺序与输入一致）" << endl;
    unordered_map<string, unordered_map<string, string>> op_table; // 运算表：op_table[a][b] = a*b
    bool input_valid = true;

    for (const string& a : setG) {
        for (const string& b : setG) {
            cout << "请输入 " << a << " * " << b << " = ";
            string res = inputValidElement("");
            // 验证运算结果是否在集合中（封闭性预检查，后续会正式验证）
            if (!setG_set.count(res)) {
                cout << "警告：运算结果" << res << "不在集合中，可能违反封闭性！" << endl;
                // 仍允许输入（后续会判定为不构成群）
            }
            op_table[a][b] = res;
        }
    }

    // 步骤4：验证4个群公理
    bool is_group = true;
    string identity = "";  // 存储单位元

    // 公理1：验证封闭性
    cout << "\n===== 开始验证群公理 =====" << endl;
    bool closure = true;
    for (const string& a : setG) {
        for (const string& b : setG) {
            string res = op_table[a][b];
            if (!setG_set.count(res)) {
                closure = false;
                cout << "公理1（封闭性）：不满足！" << a << " * " << b << " = " << res << "（不在集合中）" << endl;
                break;
            }
        }
        if (!closure) break;
    }
    if (closure) cout << "公理1（封闭性）：满足" << endl;
    else is_group = false;

    // 公理2：验证结合律（有限集合需遍历所有三元组）
    bool associativity = true;
    if (is_group) {
        for (const string& a : setG) {
            for (const string& b : setG) {
                for (const string& c : setG) {
                    string left = op_table[op_table[a][b]][c];  // (a*b)*c
                    string right = op_table[a][op_table[b][c]]; // a*(b*c)
                    if (left != right) {
                        associativity = false;
                        cout << "公理2（结合律）：不满足！(" << a << "*" << b << ")*" << c << " = " << left
                            << "，但" << a << "*(" << b << "*" << c << ") = " << right << endl;
                        break;
                    }
                }
                if (!associativity) break;
            }
            if (!associativity) break;
        }
        if (associativity) cout << "公理2（结合律）：满足" << endl;
        else is_group = false;
    }

    // 公理3：验证单位元存在
    bool has_identity = false;
    if (is_group) {
        for (const string& e : setG) {
            bool is_identity = true;
            for (const string& a : setG) {
                if (op_table[e][a] != a || op_table[a][e] != a) {
                    is_identity = false;
                    break;
                }
            }
            if (is_identity) {
                identity = e;
                has_identity = true;
                break;
            }
        }
        if (has_identity) cout << "公理3（单位元存在）：满足，单位元为 " << identity << endl;
        else {
            cout << "公理3（单位元存在）：不满足！" << endl;
            is_group = false;
        }
    }

    // 公理4：验证每个元素都有逆元
    bool has_inverses = false;
    if (is_group) {
        has_inverses = true;
        cout << "公理4（逆元存在）：" << endl;
        for (const string& a : setG) {
            bool found_inverse = false;
            for (const string& b : setG) {
                if (op_table[a][b] == identity && op_table[b][a] == identity) {
                    cout << "  元素" << a << "的逆元为 " << b << endl;
                    found_inverse = true;
                    break;
                }
            }
            if (!found_inverse) {
                cout << "  元素" << a << "无逆元！" << endl;
                has_inverses = false;
                break;
            }
        }
        if (!has_inverses) is_group = false;
    }

    // 最终结果
    cout << "\n===== 判定结果 =====" << endl;
    if (is_group) {
        cout << "该集合关于给定的二元运算构成群！" << endl;
        cout << "集合：G = {" << setG[0];
        for (int i = 1; i < n; ++i) cout << ", " << setG[i];
        cout << "}" << endl;
        cout << "二元运算：*（运算表已输入）" << endl;
        cout << "单位元：" << identity << endl;
    }
    else {
        cout << "该集合关于给定的二元运算不构成群！" << endl;
    }

    return 0;
}