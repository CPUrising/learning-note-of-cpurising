#include <iostream>
#include <fstream> //for is_open() rdbuf() close()etc other.files 
#include <string>
#include <sstream>//make stringstream rdbuf()
#include <cctype>  //for isspace ispunct

int main() {
    //a file to be a string
    std::ifstream file("example.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open the file" << std::endl;
        return 0;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();//close right after reading
    std::string content = buffer.str();//content stores all of txt and changes to string

    std::string target;
    target = "that";
    int count = 0;
    std::string currentWord;
    for (char c : content)
    {
        if (isspace(c) || ispunct(c))
        {
            if (currentWord == target)
                count++;
            currentWord.clear();
        }
        else
        {
            c = tolower(c);
            currentWord += c;
        }
    }
    if (currentWord == target)
        count++;
    std::cout << "there are " << count << " '"<<target<<"' in the passage.\n";
    return 0;
}
