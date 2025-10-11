#include <vector>
#include <iostream>
using namespace std;

template<typename T>//a typename only for a function
//unable pointer++
T* find3(const vector<T>& vec, T value) {
    //size_t>=0!!!and very long
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == value) {
            return const_cast<T*>(&vec[i]);
        }
    }
    return 0;
}
template <typename ElemType>
ElemType* find2(const vector<ElemType>& vec, const ElemType& Value)
{
    // 改用size_t避免类型不匹配，添加const_cast处理const指针
    for (size_t iX = 0; iX < vec.size(); iX++)
    {
        if (vec[iX] == Value)
        {
            return const_cast<ElemType*>(&vec[iX]);
        }
    }
    return nullptr;
}
//no const is easier
template<typename T>
T* find(std::vector<T>& arr, T v)
{
    typename vector<T>::iterator it = arr.begin();
    for (; it != arr.end(); ++it)
    {
        if (*it == v) 
            return &(*it);
    }
    return 0;
}
int main() {
    std::vector<int> numbers = { 5, 2, 9, 1, 5, 6 };
    std::vector<double> floating = { 5.0, 2.0, 9.0, 1.0, 5.3, 6.3 };

    int* result1 = find(numbers, 9);
    if (result1) {
        std::cout << "Found value: " << *result1 << std::endl;
        *result1 = 99; 
        std::cout << "Modified value: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "Value 9 not found" << std::endl;
    }

    int* result2 = find(numbers, 3);
    if (result2) {
        std::cout << "Found value: " << *result2 << std::endl;
    }
    else {
        std::cout << "Value 3 not found" << std::endl;
    }

    double* result3 = find(floating, 6.3);
    if (result3) {
        std::cout << "Found value: " << *result3 << std::endl;
    }
    else {
        std::cout << "Value 6.3 not found" << std::endl;
    }

    return 0;
}
