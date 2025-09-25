#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>

//new��ʵ�������...
class DynamicString {
private:
    char* str;  // ָ��̬������ַ���

public:
    // ���캯�������붯̬�ڴ�
    DynamicString(const char* initialStr = "") {
        // Ϊ�ַ��������ڴ棨��1��Ϊ�˴洢��ֹ��'\0'��
        str = new char[std::strlen(initialStr) + 1];

        // �����ַ�������
        std::strcpy(str, initialStr);

        std::cout << "���캯���������� " << (std::strlen(initialStr) + 1) << " �ֽ��ڴ�" << std::endl;
    }

    // �����������ͷŶ�̬�ڴ�
    ~DynamicString() {
        if (str != nullptr) {
            delete[] str;  // �ͷŶ�̬���������
            str = nullptr;  // ����Ұָ��
            std::cout << "�����������ͷ����ڴ�" << std::endl;
        }
    }

    // ������������ȡ�ַ���
    const char* getString() const {
        return str;
    }

    // �����������޸��ַ���
    void setString(const char* newStr) {
        // ���ͷž��ڴ�
        delete[] str;

        // �������ڴ沢��������
        str = new char[std::strlen(newStr) + 1];
        std::strcpy(str, newStr);
    }
};

int main() {
    // �������󣬴������캯��
    DynamicString str1("Hello, World!");
    std::cout << "str1: " << str1.getString() << std::endl;

    // �޸��ַ�������
    str1.setString("Dynamic memory management");
    std::cout << "�޸ĺ� str1: " << str1.getString() << std::endl;

    // ������һ������
    DynamicString str2;
    str2.setString("Another string");
    std::cout << "str2: " << str2.getString() << std::endl;

    // �������ʱ������ᱻ���٣�������������
    return 0;
}