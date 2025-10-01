#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

int main() {
    std::ifstream file("example.txt");

    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    // 将文件内容读入字符串流
    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();//close right after reading
    
    std::string content = buffer.str();//content stores all of txt 转换为字符串

    //依次遍历，以空格为一次string的读取记录，然后更新
    return 0;
}
