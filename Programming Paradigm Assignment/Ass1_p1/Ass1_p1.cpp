#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main() {
    std::ifstream file("example.txt");

    if (!file.is_open()) {
        std::cerr << "�޷����ļ�" << std::endl;
        return 1;
    }

    // ���ļ����ݶ����ַ�����
    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();//close right after reading
    
    std::string content = buffer.str();//content stores all of txt ת��Ϊ�ַ���

    //���α������Կո�Ϊһ��string�Ķ�ȡ��¼��Ȼ�����
    return 0;
}
