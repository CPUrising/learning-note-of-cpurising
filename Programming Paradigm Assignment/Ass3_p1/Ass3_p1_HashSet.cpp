//2458620 �ź� ���ں�ڷ�ʡ��
#include <iostream>
#include<vector>
#include <unordered_set>
using namespace std;

bool isIncludeRepeat(vector<int>& arr, int k)
{
	int len = arr.size();
	if (len < k||k<=0)
		return false;

	unordered_set<int> window;
	for (int i = 0; i < k; ++i)
	{
		if (window.find(arr[i]) != window.end())
			return true;
		else
		{
			window.insert(arr[i]);
		}
	}

	for (int i = k; i < len; ++i)
	{
		window.erase(arr[i - k]);
		if (window.find(arr[i]) != window.end())
			return true;
		else
		{
			window.insert(arr[i]);
		}
	}
	return false;
}
bool inputNum(int& num) {
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
void inputVector(vector<int> &arr)
{
	while (1)
	{
		int temp;
		if(!inputNum(temp))
			break;
		arr.push_back(temp);
	}
}
int main()
{
	int k;
	vector<int> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input the length of the window:\n";
	inputNum(k);
	if (isIncludeRepeat(arr, k))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
