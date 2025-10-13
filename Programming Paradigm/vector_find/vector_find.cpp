#include <vector>
#include <iostream>
#include <list>
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

template <typename ElemType>
inline ElemType* begin(const vector<ElemType>& vec)
{
    return vec.empty() ? 0 : &vec[0];
}
template <typename ElemType>
inline ElemType* end(const vector<ElemType>& vec)
{
    return vec.empty() ? 0 : ++(&vec[vec.size()-1]);
}
template <typename ElemType>
ElemType* find3_5(const ElemType* first, const ElemType* last, const ElemType& Value)
{
    if ((!first) || (!last))  return 0;

    for (; first != last; first++)
    {
        if (*first == Value) return const_cast<ElemType*>(first);
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

    // ԭ�еĲ��Դ���
    int* result1 = find1(numbers, 9);
    if (result1) {
        std::cout << "find1 - Found value: " << *result1 << std::endl;
        *result1 = 99;
        std::cout << "find1 - Modified value: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "find1 - Value 9 not found" << std::endl;
    }

    // ���find3_5�Ĳ��԰���
    std::cout << "\nTesting find3_5 function:" << std::endl;

    // 1. ����vector��ͨ��data()��ȡ�ײ�����ָ�룩
    int* vec_result = find3_5(numbers.data(), numbers.data() + numbers.size(), 99);
    if (vec_result) {
        std::cout << "find3_5 - Found in vector: " << *vec_result
            << " at position: " << (vec_result - numbers.data()) << std::endl;

        // �޸��ҵ���Ԫ��
        *vec_result = 999;
        std::cout << "find3_5 - Modified vector element: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "find3_5 - Value 99 not found in vector" << std::endl;
    }

    // 2. ����double����vector
    double* dbl_result = find3_5(floating.data(), floating.data() + floating.size(), 5.3);
    if (dbl_result) {
        std::cout << "find3_5 - Found in double vector: " << *dbl_result
            << " at position: " << (dbl_result - floating.data()) << std::endl;
    }
    else {
        std::cout << "find3_5 - Value 5.3 not found in double vector" << std::endl;
    }

    // 3. ����ԭ��int����
    int int_array[] = { 10, 20, 30, 40, 50 };
    int* arr_result = find3_5(int_array, int_array + 5, 30);
    if (arr_result) {
        std::cout << "find3_5 - Found in int array: " << *arr_result
            << " at index: " << (arr_result - int_array) << std::endl;
    }
    else {
        std::cout << "find3_5 - Value 30 not found in int array" << std::endl;
    }
    //4.���Կ�vector
    //vector<string> svec;
    //string* it = find3_5(svec.data(), svec.data() + svec.size(), "0");
    // ���find4�Ĳ��԰��������ֲ��䣩
    std::cout << "\nTesting find4 function:" << std::endl;

    auto it_vec = find4(numbers.begin(), numbers.end(), 999);
    if (it_vec != numbers.end()) {
        std::cout << "find4 - Found in vector: " << *it_vec << " at position: " << (it_vec - numbers.begin()) << std::endl;
        *it_vec = 9999;
        std::cout << "find4 - Modified vector element: " << numbers[2] << std::endl;
    }
    else {
        std::cout << "find4 - Value 999 not found in vector" << std::endl;
    }

    auto it_double = find4(floating.begin(), floating.end(), 5.3);
    if (it_double != floating.end()) {
        std::cout << "find4 - Found in double vector: " << *it_double << " at position: " << (it_double - floating.begin()) << std::endl;
    }
    else {
        std::cout << "find4 - Value 5.3 not found in double vector" << std::endl;
    }

    int int_array2[] = { 10, 20, 30, 40, 50 };
    int array_size = sizeof(int_array2) / sizeof(int_array2[0]);

    auto it_int_arr = find4(int_array2, int_array2 + array_size, 30);
    if (it_int_arr != int_array2 + array_size) {
        std::cout << "find4 - Found in int array: " << *it_int_arr
            << " at index: " << (it_int_arr - int_array2) << std::endl;
    }
    else {
        std::cout << "find4 - Value 30 not found in int array" << std::endl;
    }

    const char* str_array2[] = { "apple", "banana", "cherry", "date" };
    int str_array_size = sizeof(str_array2) / sizeof(str_array2[0]);

    auto it_str_arr = find4(str_array2, str_array2 + str_array_size, "cherry");
    if (it_str_arr != str_array2 + str_array_size) {
        std::cout << "find4 - Found in string array: " << *it_str_arr
            << " at index: " << (it_str_arr - str_array2) << std::endl;
    }
    else {
        std::cout << "find4 - String 'cherry' not found in array" << std::endl;
    }

    auto it_not_found = find4(numbers.begin(), numbers.end(), 99999);
    if (it_not_found == numbers.end()) {
        std::cout << "find4 - Correctly did not find value 99999 in vector" << std::endl;
    }
    //find4 test list
    const int asize = 8;
    int ia[asize] = { 1, 1, 2, 3, 5, 8, 13, 21 };

    // ʹ�������ʼ��vector��list
    vector<int> ivec(ia, ia + asize);
    list<int> ilist(ia, ia + asize);  // ������������list -> ilist��������ؼ��ֳ�ͻ��

    // 1. ����ԭ������
    int* pa = find4(ia, ia + asize, 1024);
    if (pa != ia + asize) {
        cout << "�������ҵ�1024��λ�ã�" << (pa - ia) << endl;
    }
    else {
        cout << "������δ�ҵ�1024" << endl;
    }

    // ���������д��ڵ�Ԫ�أ���֤������ȷ�ԣ�
    int* pa2 = find4(ia, ia + asize, 5);
    if (pa2 != ia + asize) {
        cout << "�������ҵ�5��λ�ã�" << (pa2 - ia) << endl;
    }

    // 2. ����vector����
    vector<int>::iterator it_v = find4(ivec.begin(), ivec.end(), 1024);
    if (it_v != ivec.end())//test if iterator arrive at rear or behind
    {
        cout << "vector���ҵ�1024��λ�ã�" << (it_v - ivec.begin()) << endl;
    }
    else {
        cout << "vector��δ�ҵ�1024" << endl;
    }

    // 3. ����list����
    list<int>::iterator it_l = find4(ilist.begin(), ilist.end(), 1024);
    if (it_l != ilist.end()) {
        cout << "list���ҵ�1024" << endl;
        // list��������֧��������ʣ��޷�ֱ�Ӽ�������λ��
        // ����Ҫλ����Ϣ�����ֶ�����
        int pos = 0;
        for (list<int>::iterator it = ilist.begin(); it != it_l; ++it) {
            pos++;
        }
        cout << "list��1024��λ�ã�" << pos << endl;
    }
    else {
        cout << "list��δ�ҵ�1024" << endl;
    }

    // ����list�д��ڵ�Ԫ��
    list<int>::iterator it_l2 = find4(ilist.begin(), ilist.end(), 8);
    if (it_l2 != ilist.end()) {
        cout << "list���ҵ�8��ֵΪ��" << *it_l2 << endl;
        // ����λ��
        int pos = 0;
        for (list<int>::iterator it = ilist.begin(); it != it_l2; ++it) {
            pos++;
        }
        cout << "list��8��λ�ã�" << pos << endl;
    }

    return 0;
}