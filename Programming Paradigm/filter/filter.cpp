#include <iostream>
#include <vector>
#include <list>
#include <algorithm>  // 包含 find_if 算法
#include <iterator>
#include <string>
#include <functional> // 包含 bind2nd 适配器
using namespace std;

// 待测试的 filter 函数
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
     vector<int> ivec2;//一个个赋值的方式，没有足够空间error

     filter(ia, ia + iSize, ia2, 10, less<int>());

     //filter(ivec.begin(), ivec.end(),ivec2.begin(), 10, greater<int>());//一个个赋值的方式，没有足够空间error

     filter(ivec.begin(), ivec.end(), back_inserter(ivec2), 10, greater<int>());  //OK now, the assignment using iterator is replaced by insertion

}
