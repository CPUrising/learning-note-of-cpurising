//2450721 �ź� ���ں�ڷ�ʡ��
#include <iostream>
#include<assert.h>
using namespace std;
const int maxPQSize = 50;

template <class Type> 
class PQueue
{
private:
	Type* pqelements;
	int count;
public:
	PQueue(int i = 10);
	~PQueue(){ delete[] pqelements; }
	void PQueueInsert(const Type& item);
	Type PQremove();
	void makeEmpty() { count = 0; }
	int isEmpty()const
	{
		return count == 0;
	}
	int isFull()const
	{
		return count == maxPQSize;
	}
	int Length() const { return count; }
};
template <class Type>
PQueue<Type>::PQueue(int i):count(0)
{
	pqelements = new Type[maxPQSize];
	assert(pqelements != 0);
}
template <class Type>
void PQueue<Type>::PQueueInsert(const Type& item)
{
	assert(!isFull());
	pqelements[count++] = item;
}
template <class Type>
Type PQueue<Type>::PQremove()//move min
{
	assert(!isEmpty());
	Type min = pqelements[0];
	int minindex = 0;
	for (int i=1;i<count;i++)
	{
		if (min > pqelements[i])
		{
			min = pqelements[i];
			minindex = i;
		}
	}
	pqelements[minindex] = pqelements[count-1];
	count--;
	return min;
}

int main()
{
    // ����һ���������͵����ȶ���
    PQueue<int> pq;

    // ���Բ������
    cout << "����Ԫ��: 5, 2, 8, 1, 6" << endl;
    pq.PQueueInsert(5);
    pq.PQueueInsert(2);
    pq.PQueueInsert(8);
    pq.PQueueInsert(1);
    pq.PQueueInsert(6);

    // ��ʾ���г���
    cout << "���е�ǰ����: " << pq.Length() << endl;

    // �����Ƴ�������Ӧ�ð��մ�С�����˳�����
    cout << "�����Ƴ�Ԫ��: ";
    while (!pq.isEmpty())
    {
        cout << pq.PQremove() << " ";
    }
    cout << endl;

    // ���Կն���
    cout << "�Ƴ�����Ԫ�غ󣬶����Ƿ�Ϊ��: " << (pq.isEmpty() ? "��" : "��") << endl;

    // �ٴβ��벢����
    cout << "�ٴβ���Ԫ��: 3, 9, 4" << endl;
    pq.PQueueInsert(3);
    pq.PQueueInsert(9);
    pq.PQueueInsert(4);

    cout << "�ٴ��Ƴ�Ԫ��: ";
    while (!pq.isEmpty())
    {
        cout << pq.PQremove() << " ";
    }
    cout << endl;

    return 0;
}
