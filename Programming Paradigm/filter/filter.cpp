#include <iostream>
#include <vector>
#include <list>
#include <algorithm>  // ���� find_if �㷨
#include <iterator>
#include <string>
#include <functional> // ���� bind2nd ������
using namespace std;

// �����Ե� filter ����
template<typename InputIter, typename OutputIter,
    typename ElemType, typename Comp>
OutputIter filter(InputIter first, InputIter last,
    OutputIter at, const ElemType& thres, Comp pred)
{
    while ((first = find_if(first, last, bind2nd(pred, thres))) != last)
    {
        *at++ = *first++;
    }
    return at;
}

int main()
{
     const int iSize = 8;
     int ia[iSize] = {12, 8, 4, 13, 65, 3, 0, 20};
     vector<int> ivec(ia, ia + iSize);

     int ia2[iSize];//error! Will cause runtime error, because iterator will point to an illegal position
     vector<int> ivec2;//һ������ֵ�ķ�ʽ��û���㹻�ռ�error

     filter(ia, ia + iSize, ia2, 10, less<int>());

     //filter(ivec.begin(), ivec.end(),ivec2.begin(), 10, greater<int>());//һ������ֵ�ķ�ʽ��û���㹻�ռ�error

     filter(ivec.begin(), ivec.end(), back_inserter(ivec2), 10, greater<int>());  //OK now, the assignment using iterator is replaced by insertion

}
