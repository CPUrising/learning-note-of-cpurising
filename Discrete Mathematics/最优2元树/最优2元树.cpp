#include <iostream>
#include <iomanip>
// 用于动态数组（替代原固定大小数组，更灵活）
#include <vector>
using namespace std;

// 树节点结构体（C++中可直接使用struct，无需typedef）
struct TreeNode {
    int num;
    TreeNode* Lnode;
    TreeNode* Rnode;

    // 构造函数（简化节点初始化）
    TreeNode(int val) : num(val), Lnode(nullptr), Rnode(nullptr) {}
};

// 全局变量（保持原逻辑，用于存储节点和前缀码）
vector<TreeNode*> fp;
vector<char> s;

// 生成叶子节点（适配vector动态数组）
void init_node(const vector<int>& freq) {
    fp.clear();
    for (int val : freq) {
        // C++用new创建对象，自动调用构造函数
        TreeNode* pt = new TreeNode(val);
        fp.push_back(pt);
    }
}

// 排序函数（按节点权重升序排列，适配vector）
void sort_nodes() {
    int n = fp.size();
    // 简单冒泡排序（保持原排序逻辑）
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (fp[j]->num > fp[j + 1]->num) {
                swap(fp[j], fp[j + 1]);
            }
        }
    }
}

// 构造最优2元树（哈夫曼树）
TreeNode* construct_tree(const vector<int>& freq) {
    init_node(freq);
    int n = freq.size();

    // 循环构造非叶子节点
    while (fp.size() > 1) {
        // 取出权重最小的两个节点
        TreeNode* left = fp[0];
        TreeNode* right = fp[1];

        // 创建新的非叶子节点（权重为两子节点之和）
        TreeNode* parent = new TreeNode(left->num + right->num);
        parent->Lnode = left;
        parent->Rnode = right;

        // 从数组中删除已合并的两个节点
        fp.erase(fp.begin(), fp.begin() + 2);
        // 添加新节点到数组
        fp.push_back(parent);
        // 重新排序
        sort_nodes();
    }

    // 返回根节点（最后剩余的节点）
    return fp.empty() ? nullptr : fp[0];
}

// 前序遍历生成前缀码（修正原代码逻辑错误）
void preorder(TreeNode* p, int k, char c) {
    if (p == nullptr) return;

    // 记录当前路径的编码（左子树0，右子树1）
    if (k < s.size()) {
        s[k] = (c == '1') ? '0' : '1';
    }
    else {
        s.push_back((c == '1') ? '0' : '1');
    }

    // 叶子节点：输出编码
    if (p->Lnode == nullptr && p->Rnode == nullptr) {
        cout << p->num << ": ";
        for (int j = 0; j <= k; j++) {
            cout << s[j];
        }
        cout << endl;
        return;
    }

    // 递归遍历左右子树（原代码右子树参数错误，修正为'0'）
    preorder(p->Lnode, k + 1, '1');
    preorder(p->Rnode, k + 1, '0');
}

// 释放树内存（避免内存泄漏，C++必做）
void free_tree(TreeNode* root) {
    if (root == nullptr) return;
    free_tree(root->Lnode);
    free_tree(root->Rnode);
    delete root;
}
void inputNum(int& len)//cin and check
{
    while (1)
    {
        cin >> len;
        if (!cin.good() || len < 2 || len > 20)
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Please re-enter a positive integer (2-20):\n";
        }
        else
            break;
    }
}
int main() {
    int n;
    cout << "请输入节点个数(必须是正整数): ";
    inputNum(n);
    // 输入频率数据（动态数组存储，支持任意合法个数）
    vector<int> freq(n);
    cout << "请输入节点频率(以空格分隔): ";
    for (int i = 0; i < n; i++) {
        inputNum(freq[i]);
    }

    // 构造哈夫曼树
    TreeNode* root = construct_tree(freq);
    if (root == nullptr) {
        cout << "树构造失败！" << endl;
        return 1;
    }

    // 初始化前缀码存储容器
    s.clear();
    // 生成并输出前缀码（从根节点开始遍历）
    preorder(root, 0, '1');

    // 释放内存
    free_tree(root);
    return 0;
}