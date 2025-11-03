#include <iostream>
#include <cstdlib>
#include <limits>  // 用于清除输入缓冲区，处理异常输入

using namespace std;  // 简化标准库调用

// 全局变量：适配原实验矩阵尺寸设计，新增常量定义避免魔法数字
const int MAX_MATRIX_SIZE = 100;  // 矩阵最大尺寸（与原代码100保持一致）
int s[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];  // 存储关系矩阵
int z;  // 功能选择序号
int d, n, i, j;  // d=列数，n=行数，i/j=循环变量


// 函数声明：保留原功能函数名，新增输入校验辅助函数
void output(int s[][MAX_MATRIX_SIZE]);
void zifan(int s2[][MAX_MATRIX_SIZE]);
void duichen(int s2[][MAX_MATRIX_SIZE]);
void chuandi(int s2[][MAX_MATRIX_SIZE]);
void select();
void exitFunc();  // 重命名避免与std::exit冲突
void clearInputBuffer();  // 清除输入缓冲区，处理异常输入残留


int main() {
    select();
    return 0;
}

// 清除输入缓冲区：解决非整数输入后cin陷入错误状态的问题
void clearInputBuffer() {
    cin.clear();  // 重置cin状态，清除错误标记
    // 忽略缓冲区中所有字符，直到换行符（避免残留字符干扰后续输入）
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// 选择功能与矩阵输入：核心强化输入合法性校验
void select() {
    // 1. 校验矩阵行数输入（范围：1~MAX_MATRIX_SIZE，仅允许整数）
    cout << "请输入矩阵的行数（1~" << MAX_MATRIX_SIZE << "）：";
    while (!(cin >> n) || n < 1 || n > MAX_MATRIX_SIZE) {
        clearInputBuffer();
        cout << "输入错误！行数必须是1~" << MAX_MATRIX_SIZE << "的整数，请重新输入：";
    }

    // 2. 校验矩阵列数输入（范围：1~MAX_MATRIX_SIZE，提示“关系矩阵通常为方阵”）
    cout << "请输入矩阵的列数（1~" << MAX_MATRIX_SIZE << "，关系矩阵建议为方阵）：";
    while (!(cin >> d) || d < 1 || d > MAX_MATRIX_SIZE) {
        clearInputBuffer();
        cout << "输入错误！列数必须是1~" << MAX_MATRIX_SIZE << "的整数，请重新输入：";
    }
    clearInputBuffer();  // 清除换行符，避免影响后续元素输入

    // 3. 校验矩阵元素输入（仅允许0或1，逐元素校验）
    cout << "请输入关系矩阵（元素仅允许0或1，每行元素用空格分隔）：\n";
    for (i = 0; i < n; i++) {
        cout << "请输入矩阵的第" << i << "行元素（共" << d << "个）：";
        for (j = 0; j < d; j++) {
            // 拦截非整数或非0/1的元素
            while (!(cin >> s[i][j]) || (s[i][j] != 0 && s[i][j] != 1)) {
                clearInputBuffer();
                cout << "元素错误！第" << i << "行第" << j << "个元素必须是0或1，请重新输入该行：\n";
                cout << "请输入矩阵的第" << i << "行元素（共" << d << "个）：";
                j = 0;  // 重置当前行的输入索引，重新输入整行
            }
        }
    }

    // 4. 校验功能选择输入（仅允许1~4的整数）
    cout << "输入对应序号选择算法：\n1:自反闭包\n2:传递闭包\n3:对称闭包\n4:退出\n";
    while (!(cin >> z) || z < 1 || z > 4) {
        clearInputBuffer();
        cout << "输入错误！请输入1~4的整数选择算法：";
    }

    // 功能分支（与原逻辑一致）
    switch (z) {
    case 1: zifan(s); break;
    case 2: chuandi(s); break;
    case 3: duichen(s); break;
    case 4: exitFunc(); break;
    }
}

// 输出矩阵（与原逻辑一致，优化换行格式）
void output(int s[][MAX_MATRIX_SIZE]) {
    cout << "所求关系矩阵为：\n";
    for (i = 0; i < n; i++) {
        for (j = 0; j < d; j++) {
            cout << s[i][j] << " ";  // 增加空格分隔，提升可读性
        }
        cout << endl;
    }
}

// 求自反闭包（原逻辑不变，新增非方阵提示）
void zifan(int s2[][MAX_MATRIX_SIZE]) {
    // 自反闭包仅对“方阵”有效（关系定义于同一集合），非方阵提示异常
    if (n != d) {
        cout << "提示：仅方阵可求自反闭包（非方阵无主对角线），无法计算！\n";
        select();  // 返回重新选择
        return;
    }
    // 主对角线置1（原核心逻辑）
    for (i = 0; i < n; i++) {
        s2[i][i] = 1;
    }
    output(s2);
    select();
}

// 求对称闭包（原逻辑不变，确保转置矩阵维度合法）
void duichen(int s2[][MAX_MATRIX_SIZE]) {
    int s1[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    // 构建原矩阵的转置矩阵（确保转置维度匹配）
    for (i = 0; i < n; i++) {
        for (j = 0; j < d; j++) {
            s1[j][i] = s2[i][j];
        }
    }
    // 逻辑加（元素>1置1，原核心逻辑）
    for (i = 0; i < n; i++) {
        for (j = 0; j < d; j++) {
            s2[i][j] += s1[i][j];
            if (s2[i][j] > 1) {
                s2[i][j] = 1;
            }
        }
    }
    output(s2);
    select();
}

// 求传递闭包（原逻辑不变，新增非方阵提示）
void chuandi(int s2[][MAX_MATRIX_SIZE]) {
    // 传递闭包仅对“方阵”有效，非方阵提示异常
    if (n != d) {
        cout << "提示：仅方阵可求传递闭包（非方阵不符合关系传递性定义），无法计算！\n";
        select();  // 返回重新选择
        return;
    }

    int m[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], a[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE], k, h;
    int t[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    // 初始化辅助矩阵（原核心逻辑）
    for (i = 0; i < n; i++) {
        for (j = 0; j < d; j++) {
            a[i][j] = 0;
            t[i][j] = s2[i][j];
            m[i][j] = s2[i][j];
        }
    }
    // 传递闭包计算（原核心逻辑）
    for (h = 0; h < n; h++) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < d; j++) {
                if (m[i][j] == 1) {
                    for (k = 0; k < n; k++) {
                        if (s2[j][k] == 1) {
                            a[i][k] = 1;
                        }
                    }
                }
            }
        }
        // 更新辅助矩阵与结果矩阵
        for (i = 0; i < n; i++) {
            for (j = 0; j < d; j++) {
                m[i][j] = a[i][j];
                t[i][j] += a[i][j];
                a[i][j] = 0;
                if (t[i][j] > 1) {
                    t[i][j] = 1;
                }
            }
        }
    }
    output(t);
    select();
}

// 退出函数（重命名避免与std::exit冲突，原逻辑不变）
void exitFunc() {
    cout << "程序已退出！" << endl;
    exit(1);
}