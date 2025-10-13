//2450721 �ź� ���ں�ڷ�ʡ��
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
	void EnQueue(const Type& item); //���
	Type DeQueue(); //������
	Type GetFront(); //ȡ��ͷԪ��ֵ
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

	cout << "���г���: " << q.Length() << endl; // ���3
	cout << "��ͷԪ��: " << q.GetFront() << endl; // ���10

	q.DeQueue();
	cout << "���Ӻ��ͷԪ��: " << q.GetFront() << endl; // ���20

	return 0;
}
