//2458620 �ź� ���ں�ڷ�ʡ��
#include <iostream>
#include<vector>
#include <unordered_map>
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

bool isMoreF(vector<long long>& arr, int k, int f)//k=length of window, f=frequency
{
	unordered_map<long long, int> window;
	int left = 0;
	int right = 0;
	int len = arr.size();
	for (; right < len; ++right)
	{
		window[arr[right]]++;
		if (right - left+1 >k)
		{
			window[arr[left]]--;
			if (window[arr[left]] == 0)
				window.erase(arr[left]);
			left++;
		}
		if (window[arr[right]] >= f)
			return true;
	}
	return false;
}
int main()
{
	long long k, f;
	vector<long long> arr;
	cout << "Please input the array (enter 'q' to end):\n";
	inputVector(arr);
	cout << "Please input k,f:\n";
	inputNum(k);
	inputNum(f);
	if (isMoreF(arr, k, f))
		cout << "true\n";
	else
		cout << "false\n";
	return 0;
}
