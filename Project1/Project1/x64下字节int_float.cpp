#include <iostream>
#include <climits>
using namespace std;

int main()
{
	cout << sizeof(int) << endl;
	cout << sizeof(float) << endl;
	cout << sizeof(double) << endl;

	cout << "ʹ�ñ�׼�� <climits> ���int��Χ��" << endl;
	cout << "int ��Сֵ: " << INT_MIN << endl;
	cout << "int ���ֵ: " << INT_MAX << endl;
	cout << "int λ��: " << sizeof(int) * 8 << " λ" << endl << endl;
}