#include<iostream>
#include<vector>
using namespace std;

vector<int>* fibon_seq(int iLength)
{
    // 修正错误的输出信息语法
    if (iLength <= 0 || iLength >= 1024)
    {
        cerr << "Length " << iLength
            << " not supported, reset to 8"
            << endl;
        iLength = 8;
    }

    // 不应该使用static，否则多次调用会有问题
    vector<int>* Elems = new vector<int>(iLength);

    for (int iX = 0; iX < iLength; iX++)
    {
        if (iX == 0 || iX == 1)
            (*Elems)[iX] = 1;
        else
            (*Elems)[iX] = (*Elems)[iX - 1] + (*Elems)[iX - 2];
    }
    return Elems;
}

int main()
{
    // 调用函数获取斐波那契数列
    vector<int>* fibSeq = fibon_seq(10);

    // 输出数列元素（vector不能直接用cout输出，需要遍历）
    for (int num : *fibSeq)
    {
        cout << num << " ";
    }
    cout << endl;

    // 释放动态分配的内存
    delete fibSeq;
    return 0;
}