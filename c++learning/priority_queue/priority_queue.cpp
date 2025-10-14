//2450721 古贺 终于后悔反省了
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
    // 创建一个整数类型的优先队列
    PQueue<int> pq;

    // 测试插入操作
    cout << "插入元素: 5, 2, 8, 1, 6" << endl;
    pq.PQueueInsert(5);
    pq.PQueueInsert(2);
    pq.PQueueInsert(8);
    pq.PQueueInsert(1);
    pq.PQueueInsert(6);

    // 显示队列长度
    cout << "队列当前长度: " << pq.Length() << endl;

    // 测试移除操作，应该按照从小到大的顺序输出
    cout << "依次移除元素: ";
    while (!pq.isEmpty())
    {
        cout << pq.PQremove() << " ";
    }
    cout << endl;

    // 测试空队列
    cout << "移除所有元素后，队列是否为空: " << (pq.isEmpty() ? "是" : "否") << endl;

    // 再次插入并测试
    cout << "再次插入元素: 3, 9, 4" << endl;
    pq.PQueueInsert(3);
    pq.PQueueInsert(9);
    pq.PQueueInsert(4);

    cout << "再次移除元素: ";
    while (!pq.isEmpty())
    {
        cout << pq.PQremove() << " ";
    }
    cout << endl;

    return 0;
}
