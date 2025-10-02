#include <iostream>
#include <fstream> //for is_open() rdbuf() close()etc other.files 
#include <string>
#include <sstream>//make stringstream rdbuf()
#include <cctype>  //for isspace ispunct

int main() {
    //a file to be a string
    std::string filename;
    std::cout << "Input file name to be read (e.g., stock_data.txt):\n";
    std::getline(std::cin, filename);
    std::ifstream file(filename);
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
