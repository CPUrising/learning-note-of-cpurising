#include <iostream>
#include <vector>  // ����vectorͷ�ļ�

int main() {
    // ����һ���յ�vector���洢int���ͣ�
    std::vector<int> numbers;

    //��һ�ֳ�ʼ���ķ�ʽ
    /*const int SIZE = 20;
    vector <int> Numbers(SIZE, 18);
    ��������ȫ��ʼ��18*/


    // ��vector�����Ԫ��
    numbers.push_back(10);
    numbers.push_back(20);
    numbers.push_back(30);

    // ����Ԫ�أ�������һ��ʹ��[]�������
    std::cout << "�ڶ���Ԫ��: " << numbers[1] << std::endl;

    // ʹ��at()�������ʣ�����б߽��飩
    std::cout << "������Ԫ��: " << numbers.at(2) << std::endl;

    // ��ȡvector�Ĵ�С
    std::cout << "Ԫ������: " << numbers.size() << std::endl;

    // ����vector
    std::cout << "����Ԫ��: ";
    for (int i = 0; i < numbers.size(); ++i) {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;

    // ʹ�÷�Χforѭ��������C++11�����ϣ�
    std::cout << "��Χforѭ������: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    // ����Ԫ�ص�ָ��λ��
    numbers.insert(numbers.begin() + 1, 15);  // ������1��λ�ò���15

    // ɾ�����һ��Ԫ��
    numbers.pop_back();

    // ���vector
    numbers.clear();

    return 0;
}
//std::vector ���õĳ�Ա������
//size()�����ص�ǰԪ������
//capacity()�����ص�ǰ�����������ɵ�Ԫ�����������ܴ��� size��
//push_back()����ĩβ���Ԫ��
//pop_back()��ɾ��ĩβԪ��
//clear()���������Ԫ��
//empty()���ж��Ƿ�Ϊ��
//reserve(n)��Ԥ������������ n ��Ԫ�صĿռ�
//resize(n)������������СΪ n ��Ԫ��
//��ʵ�ʿ����У�std::vector ͨ���Ƕ�̬�������ѡ�����ȱ���������ĸ�Ч�������ԣ����ṩ�˶�̬����ı����ԡ�