#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cmath>

using namespace std;
using namespace chrono;

long long compare_count = 0;  // 比较次数
long long swap_count = 0;    // 交换/移动次数

// 重置统计变量
void resetStats() {
    compare_count = 0;
    swap_count = 0;
}

// 生成随机数数组
vector<int> generateRandomArray(int n, int min = 1, int max = 100000) {
    vector<int> arr(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min, max);
    for (int i = 0; i < n; ++i) {
        arr[i] = dis(gen);
    }
    return arr;
}

// 1. 冒泡排序
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            compare_count++; // 比较次数+1
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swap_count++; // 交换次数+1
                swapped = true;
            }
        }
        if (!swapped) break; // 提前终止（已有序）
    }
}

// 2. 选择排序
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            compare_count++; // 比较次数+1
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap(arr[i], arr[min_idx]);
            swap_count++; // 交换次数+1
        }
    }
}

// 3. 直接插入排序
void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        // 移动元素
        while (j >= 0) {
            compare_count++; // 比较次数+1
            if (arr[j] > key) {
                arr[j + 1] = arr[j];
                swap_count++; // 元素移动视为交换
                j--;
            }
            else {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

// 4. 希尔排序
void shellSort(vector<int>& arr) {
    int n = arr.size();
    // 缩小增量
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 对每个增量进行插入排序
        for (int i = gap; i < n; ++i) {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap; j -= gap) {
                compare_count++; // 比较次数+1
                if (arr[j - gap] > temp) {
                    arr[j] = arr[j - gap];
                    swap_count++; // 移动次数+1
                }
                else {
                    break;
                }
            }
            arr[j] = temp;
        }
    }
}

// 5. 快速排序辅助函数（分区）
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; ++j) {
        compare_count++; // 比较次数+1
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);
            swap_count++; // 交换次数+1
        }
    }
    swap(arr[i + 1], arr[high]);
    swap_count++; // 交换次数+1
    return (i + 1);
}

// 快速排序主函数
void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// 快速排序封装（统一接口）
void quickSortWrapper(vector<int>& arr) {
    quickSort(arr, 0, arr.size() - 1);
}

// 6. 堆排序辅助函数（堆化）
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;       // 最大元素设为根
    int l = 2 * i + 1;     // 左子节点
    int r = 2 * i + 2;     // 右子节点

    compare_count++; // 比较次数+1
    if (l < n && arr[l] > arr[largest])
        largest = l;

    compare_count++; // 比较次数+1
    if (r < n && arr[r] > arr[largest])
        largest = r;

    // 如果最大元素不是根
    if (largest != i) {
        swap(arr[i], arr[largest]);
        swap_count++; // 交换次数+1

        // 递归堆化受影响的子树
        heapify(arr, n, largest);
    }
}

// 堆排序主函数
void heapSort(vector<int>& arr) {
    int n = arr.size();

    // 构建最大堆
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // 逐个提取元素
    for (int i = n - 1; i > 0; i--) {
        // 将当前根移到末尾
        swap(arr[0], arr[i]);
        swap_count++; // 交换次数+1

        // 调用堆化
        heapify(arr, i, 0);
    }
}

// 7. 归并排序辅助函数（合并）
void merge(vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // 创建临时数组
    vector<int> L(n1), R(n2);

    // 拷贝数据
    for (int i = 0; i < n1; ++i)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j)
        R[j] = arr[m + 1 + j];

    // 合并临时数组
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        compare_count++; // 比较次数+1
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        swap_count++; // 移动次数+1
        k++;
    }

    // 拷贝剩余元素
    while (i < n1) {
        arr[k] = L[i];
        swap_count++;
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        swap_count++;
        j++;
        k++;
    }
}

// 归并排序主函数
void mergeSort(vector<int>& arr, int l, int r) {
    if (l >= r)
        return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

// 归并排序封装（统一接口）
void mergeSortWrapper(vector<int>& arr) {
    mergeSort(arr, 0, arr.size() - 1);
}

// 8. 基数排序辅助函数（获取最大值）
int getMax(vector<int>& arr) {
    int max = arr[0];
    for (int i = 1; i < arr.size(); ++i) {
        compare_count++;
        if (arr[i] > max)
            max = arr[i];
    }
    return max;
}

// 基数排序辅助函数（计数排序）
void countSort(vector<int>& arr, int exp) {
    int n = arr.size();
    vector<int> output(n);
    int count[10] = { 0 };

    // 统计出现次数
    for (int i = 0; i < n; ++i)
        count[(arr[i] / exp) % 10]++;

    // 调整计数数组
    for (int i = 1; i < 10; ++i)
        count[i] += count[i - 1];

    // 构建输出数组
    for (int i = n - 1; i >= 0; --i) {
        output[count[(arr[i] / exp) % 10] - 1] = arr[i];
        swap_count++;
        count[(arr[i] / exp) % 10]--;
    }

    // 拷贝到原数组
    for (int i = 0; i < n; ++i) {
        arr[i] = output[i];
        swap_count++;
    }
}

// 基数排序主函数
void radixSort(vector<int>& arr) {
    if (arr.empty()) return;
    // 获取最大值
    int m = getMax(arr);

    // 按每位排序
    for (int exp = 1; m / exp > 0; exp *= 10)
        countSort(arr, exp);
}

// 执行排序并统计时间/次数
void runSortTest(const string& sortName, void (*sortFunc)(vector<int>&), vector<int> arr) {
    resetStats(); // 重置统计
    auto start = high_resolution_clock::now(); // 开始计时
    sortFunc(arr); // 执行排序
    auto end = high_resolution_clock::now();   // 结束计时

    // 计算耗时（毫秒）
    double duration = duration_cast<microseconds>(end - start).count() / 1000.0;

    // 输出结果
    cout << left << setw(20) << (sortName + "所用时间：")
        << right << setw(10) << fixed << setprecision(3) << duration << "\n";
    cout << left << setw(20) << (sortName + "交换次数：")
        << right << setw(10) << swap_count << "\n\n";
}

// 初始化界面
void systemInit() {
    cout << "\n        排序算法比较        " << endl;
    cout << "****************************" << endl;
    cout << "**------------------------**" << endl;
    cout << "** 1 --- 冒泡排序         **" << endl;
    cout << "** 2 --- 选择排序         **" << endl;
    cout << "** 3 --- 直接插入排序     **" << endl;
    cout << "** 4 --- 希尔排序         **" << endl;
    cout << "** 5 --- 快速排序         **" << endl;
    cout << "** 6 --- 堆排序           **" << endl;
    cout << "** 7 --- 归并排序         **" << endl;
    cout << "** 8 --- 基数排序         **" << endl;
    cout << "** 9 --- 退出程序         **" << endl;
    cout << "**------------------------**" << endl;
    cout << "****************************" << endl;
}

// 输入数组长度（校验）
void inputNum(int& len) {
    cout << "请输入要产生的随机数的个数：";
    while (1) {
        cin >> len;
        if (!cin.good() || len <= 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "输入错误！请输入正整数：";
        }
        else {
            cin.clear();
            cin.ignore(1000, '\n');
            break;
        }
    }
}

// 选择操作（校验）
char choose() {
    char choice;
    cout << "\n请选择要执行的操作（1-9）：";
    while (1) {
        cin >> choice;
        choice = toupper(choice);
        if (!cin.good() || !(choice >= '1' && choice <= '9')) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "输入错误！请选择1-9中的操作：";
        }
        else {
            cin.clear();
            cin.ignore(1000, '\n');
            break;
        }
    }
    return choice;
}

// 执行指定排序算法
void executeSort(char choice, const vector<int>& baseArr) {
    switch (choice) {
    case '1':
        runSortTest("冒泡排序", bubbleSort, baseArr);
        break;
    case '2':
        runSortTest("选择排序", selectionSort, baseArr);
        break;
    case '3':
        runSortTest("直接插入排序", insertionSort, baseArr);
        break;
    case '4':
        runSortTest("希尔排序", shellSort, baseArr);
        break;
    case '5':
        runSortTest("快速排序", quickSortWrapper, baseArr);
        break;
    case '6':
        runSortTest("堆排序", heapSort, baseArr);
        break;
    case '7':
        runSortTest("归并排序", mergeSortWrapper, baseArr);
        break;
    case '8':
        runSortTest("基数排序", radixSort, baseArr);
        break;
    default:
        break;
    }
}

int main() {
    int arrLen;
    char opChoice;

    // 初始化界面
    systemInit();
    // 输入数组长度
    inputNum(arrLen);
    // 生成随机数组
    vector<int> randomArr = generateRandomArray(arrLen);

    // 循环选择操作
    while (true) {
        opChoice = choose();
        if (opChoice == '9') {
            cout << "\n程序已退出！" << endl;
            break;
        }
        executeSort(opChoice, randomArr);
    }

    return 0;
}