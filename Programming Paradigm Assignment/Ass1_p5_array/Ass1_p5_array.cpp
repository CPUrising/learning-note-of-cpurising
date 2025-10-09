#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>
#include <vector>
#include <limits>

using namespace std;

int file_to_string(string& content)
{
    string filename;
    cout << "Input file name to be read (e.g., stock_data.txt):\n";
    getline(cin, filename);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open the file" << endl;
        return -1;
    }
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    content = buffer.str();
    return 0;
}
void array_load_from_string(string& content, vector<double>& price)
{
    double currentNumber = 0;
    double decimalFactor = 0.1;
    bool hasStarted = false;
    bool inDecimal = false;

    for (char c : content)
    {
        if (isspace(c) || ispunct(c))
        {
            if (c == '.' && !inDecimal && hasStarted)
            {
                inDecimal = true;
                continue;
            }

            if (hasStarted)
            {
                price.push_back(currentNumber);
                currentNumber = 0;
                decimalFactor = 0.1;
                hasStarted = false;
                inDecimal = false;
            }
        }
        else if (isdigit(c))
        {
            int digit = c - '0';
            if (!inDecimal)
            {
                currentNumber = currentNumber * 10 + digit;
            }
            else
            {
                currentNumber += digit * decimalFactor;
                decimalFactor *= 0.1;
            }
            hasStarted = true;
        }
    }
    if (hasStarted)
    {
        price.push_back(currentNumber);
    }
}
//find maxProfit
double maxProfit(vector<double>& prices) {
    if (prices.empty()) return 0;

    double minPrice = prices[0];
    double maxProfit = 0;

    for (double price : prices) {
        minPrice = min(minPrice, price);
        double currentProfit = price - minPrice;
        if (currentProfit > maxProfit) {
            maxProfit = currentProfit;
        }
    }
    return maxProfit;
}
int main()
{
    int dayBuy = 0;
    string content;

    if (file_to_string(content) == -1)
    {
        return 0;
    }

    vector<double> price;
    array_load_from_string(content, price);
    cout << "maxProfit: " << maxProfit(price) << endl;

    return 0;
}