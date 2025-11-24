//2458620 古贺 终于后悔反省了
#include <iostream>
#include <vector>
using namespace std;

class MinHeap
{
private:
	vector<int>heap;
	int size;

public:
	MinHeap(int capacity = 10000) {
		heap.reserve(capacity);
		size = 0;
	}

	void heapifyUp(int idx)
	{
		while (idx > 0) {
			int parent = (idx - 1) / 2;
			if (heap[parent] <= heap[idx])
				break;
			swap(heap[parent], heap[idx]);
			idx = parent;
		}
	}
	void heapifyDown(int idx)
	{
		while (1)
		{
			int smallest = idx;
			int left = idx * 2 + 1;
			int right = idx * 2 + 2;
			if (left<size && heap[smallest]>heap[left])
				smallest = left;
			if (right<size && heap[smallest]>heap[right])
				smallest = right;
			if (smallest == idx)
				break;
			swap(heap[idx], heap[smallest]);
			idx = smallest;
		}
	}
	void insert(int n)
	{
		heap.push_back(n);
		size++;
		heapifyUp(size-1);
	}
	int pop()
	{
		if (isEmpty()) return -1;
		int top = heap[0];
		heap[0] = heap[--size];
		heap.pop_back();
		heapifyDown(0);
		return top;
	}
	bool isEmpty()
	{
		return size == 0;
	}
};

void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good()||len<1||len>10000)
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Please re-enter a positive integer:\n";
		}
		else
			break;
	}
}

int main()
{
	int N;
	long long totalCost = 0;
	vector<int>block;
	cout << "请输入N（1≤N≤10000）：";
	inputNum(N);
	cout << "请输入" << N << "个正整数（木头长度）：";
	block.resize(N);
	for (int i = 0; i < N; ++i) {
		cin >> block[i];
	}
	MinHeap wood(N);
	for (int len : block) {
		wood.insert(len);
	}
	while (N--> 1) {
		int a = wood.pop();
		int b = wood.pop();
		int cost = a + b;
		totalCost += cost;
		wood.insert(cost);
	}
	cout << "最小锯木花费：" << totalCost << endl;
	return 0;
}
