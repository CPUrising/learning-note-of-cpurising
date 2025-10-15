#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

int main()
{
	std::vector<std::string> words;
	std::string input;
	cout << "Input a series of string and seprate by \"enter\"\n";

    while (true) {
        getline(cin, input);//seprate by "enter"
        if (input.empty()) {
            break;
        }
        words.push_back(input);
    }

    sort(words.begin(), words.end());

    for (string word : words)
        cout << word << endl;
}