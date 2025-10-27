//2458620 �ź� ���ں�ڷ�ʡ��
#include <iostream>
#include<vector>
#include<set>
using namespace std;

bool inputNum(long long& num) {
	cin >> num;
	// ���1������������������������������0��
	if (!cin.fail()) {
		return true;
	}
	// ���2���������������'q'������Ϊ������־
	else {
		cin.clear(); // �������״̬
		cin.ignore(1000, '\n'); // ���Ի�����ʣ������
		return false;
	}
}
void inputVector(vector<long long>& arr)
{
	while (1)
	{
		long long temp;
		if (!inputNum(temp))
			break;
		arr.push_back(temp);
	}
}
//set�Զ���������
bool isSmallerThanK(vector<long long>& arr,int k,int t)
{
	int n = arr.size();
	set<long long> window;
	for (int i = 0; i < n; ++i)
	{
		set <long long>::iterator it = window.lower_bound(arr[i] - t);//��߽�
		if (it != window.end() && *it <= arr[i] + t)//�Ƿ�����ұ߽�
			return true;
		else
			window.insert(arr[i]);
		if (window.size() > k)
			window.erase(arr[i - k]);
	}
	return false;
}
int main()
{
	long long k, t;
	vector<long long> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input k,t:\n";
	inputNum(k);
	inputNum(t);
	if (isSmallerThanK(arr, k,t))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
