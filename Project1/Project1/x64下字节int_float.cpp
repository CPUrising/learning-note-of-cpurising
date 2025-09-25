#include <iostream>
#include <climits>
using namespace std;

int main()
{
	cout << sizeof(int) << endl;
	cout << sizeof(float) << endl;
	cout << sizeof(double) << endl;

	cout << "使用标准库 <climits> 检测int范围：" << endl;
	cout << "int 最小值: " << INT_MIN << endl;
	cout << "int 最大值: " << INT_MAX << endl;
	cout << "int 位数: " << sizeof(int) * 8 << " 位" << endl << endl;
}