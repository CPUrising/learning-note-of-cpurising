#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

// 树节点结构体
struct TreeNode {
    int num;
    TreeNode* Lnode;
    TreeNode* Rnode;
    TreeNode(int val) : num(val), Lnode(nullptr), Rnode(nullptr) {}
};

// 全局变量（存储节点和前缀码）
vector<TreeNode*> fp;
vector<char> s;

// 生成叶子节点
void init_node(const vector<int>& freq) {
    fp.clear();
    for (int val : freq) {
        TreeNode* pt = new TreeNode(val);
        fp.push_back(pt);
    }
}

// 排序函数（按节点权重升序）
void sort_nodes() {
    int n = fp.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (fp[j]->num > fp[j + 1]->num) {
                swap(fp[j], fp[j + 1]);
            }
        }
    }
}

// 构造哈夫曼树
TreeNode* construct_tree(const vector<int>& freq) {
    init_node(freq);
    int n = freq.size();

    while (fp.size() > 1) {
        TreeNode* left = fp[0];
        TreeNode* right = fp[1];
        TreeNode* parent = new TreeNode(left->num + right->num);
        parent->Lnode = left;
        parent->Rnode = right;

        fp.erase(fp.begin(), fp.begin() + 2);
        fp.push_back(parent);
        sort_nodes();
    }

    return fp.empty() ? nullptr : fp[0];
}

// 前序遍历生成前缀码
void preorder(TreeNode* p, int k, char c) {
    if (p == nullptr) return;

    if (k < s.size()) {
        s[k] = (c == '1') ? '0' : '1';
    }
    else {
        s.push_back((c == '1') ? '0' : '1');
    }

    if (p->Lnode == nullptr && p->Rnode == nullptr) {
        cout << p->num << ": ";
        for (int j = 0; j <= k; j++) {
            cout << s[j];
        }
        cout << endl;
        return;
    }

    preorder(p->Lnode, k + 1, '1');
    preorder(p->Rnode, k + 1, '0');
}

// 释放树内存
void free_tree(TreeNode* root) {
    if (root == nullptr) return;
    free_tree(root->Lnode);
    free_tree(root->Rnode);
    delete root;
}

// 通用输入验证函数（支持任意分隔符，仅验证数值合法性）
void inputValidNum(int& num, const string& prompt, int minVal = 2, int maxVal = 20) {
    while (true) {
        // 仅当输入流出错时，清空并提示重新输入（兼容空格/换行分隔）
        if (!cin) {
            cin.clear();                  // 重置输入流状态
            cin.ignore(1000, '\n');       // 忽略剩余错误输入
            cout << prompt << endl;
        }

        // 尝试读取数值，成功则验证范围
        if (cin >> num) {
            if (num >= minVal && num <= maxVal) {
                break; // 输入合法，退出循环
            }
            else {
                cout << "数值超出范围(" << minVal << "-" << maxVal << ")，请重新输入：" << endl;
            }
        }
        else {
            cout << "输入不是有效整数，请重新输入：" << endl;
        }
    }
}

// 专用：输入节点个数（范围2-20）
void inputNodeCount(int& len) {
    cout << "请输入节点个数(必须是2-20的正整数)：";
    inputValidNum(len, "请重新输入2-20之间的正整数", 2, 20);
}

// 专用：输入频率值（范围1-INT_MAX，仅限制最小为1）
void inputFreqValue(int& freq) {
    inputValidNum(freq, "请重新输入正整数频率", 1, INT_MAX);
}

int main() {
    int n;
    // 1. 输入节点个数
    inputNodeCount(n);

    // 2. 输入节点频率（支持空格/换行任意分隔）
    vector<int> freq(n);
    cout << "请输入" << n << "个节点频率（可空格/换行分隔）：" << endl;
    for (int i = 0; i < n; i++) {
        inputFreqValue(freq[i]);
    }

    // 3. 构造哈夫曼树
    TreeNode* root = construct_tree(freq);
    if (root == nullptr) {
        cout << "树构造失败！" << endl;
        return 1;
    }

    // 4. 生成并输出前缀码
    s.clear();
    cout << "\n哈夫曼编码结果：" << endl;
    preorder(root, 0, '1');

    // 5. 释放内存
    free_tree(root);
    return 0;
}