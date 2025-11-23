#include <iostream>
#include <vector>
using namespace std;

const int MAX_SIZE = 100;  // 集合元素最大数量

// 查找元素 x 在数组 a 中的索引，-1 表示未找到
int findIndex(const int a[], int size, int x) {
    for (int i = 0; i < size; i++) {
        if (a[i] == x) {
            return i;
        }
    }
    return -1;
}

// 检查封闭性：运算结果是否都在集合中
bool checkClosure(const int a[], const int opTable[MAX_SIZE][MAX_SIZE], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int result = opTable[i][j];
            if (findIndex(a, size, result) == -1) {
                cout << "不满足封闭性！运算结果 " << result << " 不在集合中\n";
                return false;
            }
        }
    }
    cout << "满足封闭性！\n";
    return true;
}

// 检查结合律：对于 a,b,c 是否满足 (a*b)*c = a*(b*c)
bool checkAssociativity(const int a[], const int opTable[MAX_SIZE][MAX_SIZE], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            for (int k = 0; k < size; k++) {
                // 计算 (a[i] * a[j]) * a[k]
                int idx1 = findIndex(a, size, opTable[i][j]);
                int leftResult = opTable[idx1][k];

                // 计算 a[i] * (a[j] * a[k])
                int idx2 = findIndex(a, size, opTable[j][k]);
                int rightResult = opTable[i][idx2];

                if (leftResult != rightResult) {
                    cout << "不满足结合律！例如(" << a[i] << "*" << a[j] << ")*" << a[k]
                        << " = " << leftResult << " 与 " << rightResult << " = " << a[i] << "*(" << a[j] << "*" << a[k] << ")\n";
                    return false;
                }
            }
        }
    }
    cout << "满足结合律！\n";
    return true;
}

// 查找左单位元：满足 e*x = x 对所有 x 成立
vector<int> findLeftIdentity(const int a[], const int opTable[MAX_SIZE][MAX_SIZE], int size) {
    vector<int> identities;
    for (int eIdx = 0; eIdx < size; eIdx++) {
        bool isIdentity = true;
        for (int xIdx = 0; xIdx < size; xIdx++) {
            if (opTable[eIdx][xIdx] != a[xIdx]) {
                isIdentity = false;
                break;
            }
        }
        if (isIdentity) {
            identities.push_back(a[eIdx]);
        }
    }
    return identities;
}

// 检查左逆元：是否每个元素都有左逆元，相对于指定左单位元 e
bool checkLeftInverse(const int a[], const int opTable[MAX_SIZE][MAX_SIZE], int size, int e) {
    int eIdx = findIndex(a, size, e);
    if (eIdx == -1) {
        cout << "指定的单位元 " << e << " 不在集合中\n";
        return false;
    }

    for (int xIdx = 0; xIdx < size; xIdx++) {
        bool hasInverse = false;
        for (int yIdx = 0; yIdx < size; yIdx++) {
            // 检查 y * x 是否等于 e
            if (opTable[yIdx][xIdx] == e) {
                hasInverse = true;
                break;
            }
        }
        if (!hasInverse) {
            cout << "不满足逆元！元素 " << a[xIdx] << " 没有左逆元\n";
            return false;
        }
    }
    cout << "满足逆元！每个元素都有左逆元\n";
    return true;
}

// 输入并校验整数（基础版本）
void inputNum(int& num) {
    while (true) {
        cin >> num;
        if (cin.good()) {  // 输入流状态正常
            break;
        }
        else {
            cin.clear();  // 清除错误状态
            cin.ignore(1000, '\n');  // 忽略缓冲区剩余内容
            cout << "输入无效，请重新输入一个整数：";
        }
    }
}

// 输入并校验集合元素（确保不重复）
void inputSetElement(int a[], int index) {
    while (true) {
        inputNum(a[index]);  // 先确保输入是整数
        // 检查是否与之前的元素重复
        if (findIndex(a, index, a[index]) != -1) {
            cout << "元素 " << a[index] << " 重复，请重新输入：";
        }
        else {
            break;
        }
    }
}

// 输入并校验运算表元素（确保在集合中）
void inputOpTableElement(int opTable[][MAX_SIZE], int i, int j, const int a[], int size) {
    while (true) {
        inputNum(opTable[i][j]);  // 先确保输入是整数
        // 检查运算结果是否在集合中
        if (findIndex(a, size, opTable[i][j]) == -1) {
            cout << "运算结果 " << opTable[i][j] << " 不在集合中，请重新输入：";
        }
        else {
            break;
        }
    }
}

int main() {
    int a[MAX_SIZE];          // 集合元素
    int opTable[MAX_SIZE][MAX_SIZE];  // 二元运算表（结果为元素值）
    int n;                    // 集合元素数量

    // 输入集合元素数量（1~MAX_SIZE）
    do {
        cout << "请输入集合元素数量（1~" << MAX_SIZE << "）：";
        inputNum(n);
        if (n <= 0 || n > MAX_SIZE) {
            cout << "数量必须在指定范围内，请重新输入！\n";
        }
    } while (n <= 0 || n > MAX_SIZE);

    // 输入集合元素（确保不重复）
    cout << "\n请输入集合 A 的元素（用空格分隔，元素不可重复）：\n";
    for (int i = 0; i < n; i++) {
        cout << "输入第 " << i + 1 << " 个元素：";
        inputSetElement(a, i);
    }

    // 输入运算表（确保结果在集合中）
    cout << "\n请输入 " << n << "×" << n << " 的运算表（每行输入 " << n << " 个元素）：\n";
    for (int i = 0; i < n; i++) {
        cout << "输入第 " << i + 1 << " 行：";
        for (int j = 0; j < n; j++) {
            inputOpTableElement(opTable, i, j, a, n);
        }
    }

    // 依次检查构成群的四个条件：封闭性、结合律、单位元、逆元
    cout << "\n===== 开始检查群的条件 =====\n";

    // 1. 检查封闭性
    if (!checkClosure(a, opTable, n)) {
        cout << "结论：该集合及其运算不构成群（不满足封闭性）\n";
        return 0;
    }

    // 2. 检查结合律
    if (!checkAssociativity(a, opTable, n)) {
        cout << "结论：该集合及其运算不构成群（不满足结合律）\n";
        return 0;
    }

    // 3. 检查单位元
    vector<int> leftIdentities = findLeftIdentity(a, opTable, n);
    if (leftIdentities.empty()) {
        cout << "无单位元存在\n";
        cout << "结论：该集合及其运算不构成群（无单位元）\n";
        return 0;
    }
    else if (leftIdentities.size() > 1) {
        cout << "找到多个单位元：";
        for (int e : leftIdentities) {
            cout << e << " ";
        }
        cout << "\n单位元不唯一，不符合群定义\n";
        cout << "结论：该集合及其运算不构成群（单位元不唯一）\n";
        return 0;
    }
    else {
        int e = leftIdentities[0];
        cout << "找到唯一左单位元：" << e << "\n";

        // 4. 检查逆元（基于唯一单位元）
        if (checkLeftInverse(a, opTable, n, e)) {
            cout << "\n===== 检查结果 =====\n";
            cout << "结论：该集合及其运算构成群，满足所有群的条件！\n";
        }
        else {
            cout << "结论：该集合及其运算不构成群（不满足逆元条件）\n";
        }
    }

    return 0;
}