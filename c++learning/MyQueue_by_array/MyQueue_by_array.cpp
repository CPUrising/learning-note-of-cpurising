//2450721 古贺 终于后悔反省了
#include <iostream>
#include<assert.h>
using namespace std;

template <class Type>
class Queue
{
private:
	int rear, front;
	Type* elements;
	int maxSize;
public:
	Queue(int = 10);
	~Queue() { delete[] elements; }
	void EnQueue(const Type& item); //入队
	Type DeQueue(); //出队列
	Type GetFront(); //取队头元素值
	void MakeEmpty() { front = rear = 0; }
	int IsEmpty() const
	{
		return front == rear;
	}
	int IsFull() const
	{
		return (rear + 1) % maxSize == front;
	}
	int Length() const
	{
		return (rear - front + maxSize) % maxSize;
	}
};
template <class Type>
Queue<Type>::Queue(int sz):front(0),rear(0),maxSize(sz)
{
	elements = new Type[maxSize];
	assert(elements != 0);
}
template <class Type>
void Queue<Type>::EnQueue(const Type& item)
{
	assert(!IsFull());
	rear = (rear + 1) % maxSize;
	elements[rear] = item;
}
template <class Type>
Type Queue<Type>::DeQueue()
{
	assert(!IsEmpty());
	front = (front + 1) % maxSize;
	return elements[front];
}
template <class Type>
Type Queue<Type>::GetFront()
{
	assert(!IsEmpty());
	return elements[(front + 1) % maxSize];
}

int main()
{
	Queue<int> q(5);
	q.EnQueue(10);
	q.EnQueue(20);
	q.EnQueue(30);

	cout << "队列长度: " << q.Length() << endl; // 输出3
	cout << "队头元素: " << q.GetFront() << endl; // 输出10

	q.DeQueue();
	cout << "出队后队头元素: " << q.GetFront() << endl; // 输出20

	return 0;
}
