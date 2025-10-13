#include <vector>
#include <iostream>
using namespace std;

template<typename T>
T* find1(const vector<T>& vec, T value) {
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
    for (size_t iX = 0; iX < vec.size(); iX++)
    {
        if (vec[iX] == Value)
        {
            return const_cast<ElemType*>(&vec[iX]);
        }
    }
    return nullptr;
}

template<typename T>
T* find3(std::vector<T>& arr, T v)
{
    typename vector<T>::iterator it = arr.begin();
    for (; it != arr.end(); ++it)
    {
        if (*it == v)
            return &(*it);
    }
    return 0;
}

template<typename Iterator, typename T>
Iterator find4(Iterator begin, Iterator end, const T& value) {
    for (Iterator it = begin; it != end; ++it) {
        if (*it == value) {
            return it;
        }
    }
    return end;
}

int main() {
    std::vector<int> numbers = { 5, 2, 9, 1, 5, 6 };
    std::vector<double> floating = { 5.0, 2.0, 9.0, 1.0, 5.3, 6.3 };

    // 原有的测试代码
    int* result1 = find1(numbers, 9);
    if (result1) {
        std::cout << "find1 - Found value: " << *result1 << std::endl;
        *result1 = 99;
        std::cout << "find1 - Modified value: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "find1 - Value 9 not found" << std::endl;
    }

    // 添加find4的测试案例
    std::cout << "\nTesting find4 function:" << std::endl;

    // 1. 测试vector<int>
    auto it_vec = find4(numbers.begin(), numbers.end(), 99);
    if (it_vec != numbers.end()) {
        std::cout << "find4 - Found in vector: " << *it_vec << " at position: " << (it_vec - numbers.begin()) << std::endl;

        // 修改找到的元素
        *it_vec = 999;
        std::cout << "find4 - Modified vector element: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "find4 - Value 99 not found in vector" << std::endl;
    }

    // 2. 测试vector<double>
    auto it_double = find4(floating.begin(), floating.end(), 5.3);
    if (it_double != floating.end()) {
        std::cout << "find4 - Found in double vector: " << *it_double << " at position: " << (it_double - floating.begin()) << std::endl;
    }
    else {
        std::cout << "find4 - Value 5.3 not found in double vector" << std::endl;
    }

    // 3. 测试原生int数组
    int int_array[] = { 10, 20, 30, 40, 50 };
    int array_size = sizeof(int_array) / sizeof(int_array[0]);

    auto it_int_arr = find4(int_array, int_array + array_size, 30);
    if (it_int_arr != int_array + array_size) {
        std::cout << "find4 - Found in int array: " << *it_int_arr
            << " at index: " << (it_int_arr - int_array) << std::endl;
    }
    else {
        std::cout << "find4 - Value 30 not found in int array" << std::endl;
    }

    // 4. 测试字符串数组
    const char* str_array[] = { "apple", "banana", "cherry", "date" };
    int str_array_size = sizeof(str_array) / sizeof(str_array[0]);

    auto it_str_arr = find4(str_array, str_array + str_array_size, "cherry");
    if (it_str_arr != str_array + str_array_size) {
        std::cout << "find4 - Found in string array: " << *it_str_arr
            << " at index: " << (it_str_arr - str_array) << std::endl;
    }
    else {
        std::cout << "find4 - String 'cherry' not found in array" << std::endl;
    }

    // 5. 测试未找到的情况
    auto it_not_found = find4(numbers.begin(), numbers.end(), 9999);
    if (it_not_found == numbers.end()) {
        std::cout << "find4 - Correctly did not find value 9999 in vector" << std::endl;
    }

    return 0;
}
