//2450721 古贺 终于后悔反省了
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 二叉搜索树节点结构
struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    // 构造函数
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// 二叉搜索树类
class BinarySearchTree {
private:
    TreeNode* root; // 根节点

    // 辅助函数：递归插入节点
    TreeNode* insertRecursive(TreeNode* node, int val) {
        // 找到空位置，创建新节点
        if (node == nullptr) {
            return new TreeNode(val);
        }
        // 二分思想：小于当前节点，插入左子树
        if (val < node->val) {
            node->left = insertRecursive(node->left, val);
        }
        // 大于当前节点，插入右子树（跳过重复值）
        else if (val > node->val) {
            node->right = insertRecursive(node->right, val);
        }
        return node; // 返回当前节点（维持树结构）
    }

    // 辅助函数：递归二分查找
    TreeNode* searchRecursive(TreeNode* node, int target) {
        if (node == nullptr || node->val == target) {
            return node;
        }
        else if(node->val>target)
        {
			searchRecursive(node->left, target);
        }
        else
            searchRecursive(node->right, target);
    }

    // 辅助函数：中序遍历（BST中序遍历结果为升序，用于验证）
    void inorderTraversal(TreeNode* node, vector<int>& result) {
        if (node == nullptr) return;
        inorderTraversal(node->left, result);
        result.push_back(node->val);
        inorderTraversal(node->right, result);
    }

    // 辅助函数：释放内存（后序遍历）
    void destroyTree(TreeNode* node) {
        if (node == nullptr) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    // 构造函数：初始化空树
    BinarySearchTree() : root(nullptr) {}

    // 析构函数：释放所有节点内存
    ~BinarySearchTree() {
        destroyTree(root);
    }

    // 插入节点（对外接口）
    void insert(int val) {
        root = insertRecursive(root, val);
    }

    // 迭代版二分查找（推荐，无栈溢出风险）
    TreeNode* searchIterative(int target) {
		TreeNode* current = root;
        while (current != nullptr&&current->val!=target) {
            if (current->val < target)
                current = current->right;
            else
                current = current->left;
        }
        return current;
    }
    // 递归版二分查找（对外接口）
    TreeNode* searchRecursive(int target) {
        return searchRecursive(root, target);
    }

    // 中序遍历（验证BST有序性）
    vector<int> inorderTraversal() {
        vector<int> result;
        inorderTraversal(root, result);
        return result;
    }
};

// 测试函数
int main() {
    // 1. 构建二叉搜索树
    BinarySearchTree bst;
    vector<int> insertVals = { 5, 3, 7, 2, 4, 6, 8 }; // 插入顺序不影响BST特性
    for (int val : insertVals) {
        bst.insert(val);
    }

    // 2. 验证BST（中序遍历应为升序）
    cout << "BST中序遍历（验证有序性）：";
    vector<int> inorder = bst.inorderTraversal();
    for (int val : inorder) {
        cout << val << " ";
    }
    cout << endl << endl;

    // 3. 二分查找测试
    vector<int> targets = { 4, 9, 6 }; // 存在、不存在、存在的目标值
    for (int target : targets) {
        // 迭代版查找
        TreeNode* foundIter = bst.searchIterative(target);
        cout << "迭代版查找 " << target << "："
            << (foundIter ? "找到，值为 " + to_string(foundIter->val) : "未找到") << endl;

        // 递归版查找
        TreeNode* foundRec = bst.searchRecursive(target);
        cout << "递归版查找 " << target << "："
            << (foundRec ? "找到，值为 " + to_string(foundRec->val) : "未找到") << endl;
        cout << "------------------------" << endl;
    }

    return 0;
}