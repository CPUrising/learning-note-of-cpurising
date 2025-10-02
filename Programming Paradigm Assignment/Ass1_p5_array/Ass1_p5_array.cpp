#include <iostream>
#include <fstream> // For is_open(), rdbuf(), close(), and other file operations
#include <string>
#include <sstream> // For stringstream and rdbuf()
#include <cctype>  // For isspace() and ispunct()
#include <vector>

int file_to_string(std::string &content)
{
    std::string filename;
    std::cout << "Input file name to be read (e.g., stock_data.txt):\n";
    std::getline(std::cin, filename);
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file" << std::endl;
        return -1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();// Close the file immediately after reading
    content = buffer.str();// Store all text content in 'content' and convert to string
    return 0;
}

void array_load_from_string(std::string& content, std::vector<double>& price)
{
    double currentNumber = 0;     // Current number being parsed
    double decimalFactor = 0.1;   // Weight for decimal part (0.1, 0.01, 0.001...)
    bool hasStarted = false;      // Flag indicating if number parsing has started
    bool inDecimal = false;       // Flag indicating if currently parsing decimal part

    for (char c : content)
    {
        // If the character is whitespace or punctuation (delimiter)
        if (isspace(c) || ispunct(c))
        {
            // Special handling for decimal point: only treat as delimiter if already in decimal part
            if (c == '.' && !inDecimal && hasStarted)
            {
                inDecimal = true;  // Enter decimal part parsing mode
                continue;          // Do not treat as delimiter, continue parsing
            }

            // Add the parsed number to the array only if parsing has started
            if (hasStarted)
            {
                price.push_back(currentNumber);
                // Reset all state variables
                currentNumber = 0;
                decimalFactor = 0.1;
                hasStarted = false;
                inDecimal = false;
            }
        }
        else if (isdigit(c))  // If the character is a digit
        {
            int digit = c - '0';  // Convert character to corresponding numeric value
            if (!inDecimal)
            {
                // Integer part calculation: current number * 10 + new digit
                currentNumber = currentNumber * 10 + digit;
            }
            else
            {
                // Decimal part calculation: add (digit * current weight), then reduce weight by 10x
                currentNumber += digit * decimalFactor;
                decimalFactor *= 0.1;
            }
            hasStarted = true;  // Mark that number parsing has started
        }
    }

    // Handle the last number that may remain unprocessed at the end of the string
    if (hasStarted)
    {
        price.push_back(currentNumber);
    }
}

int findLuckyday(int dayBuy, std::vector<double>& price, double& profit)
{
    int luckyday = -1;
    int len = price.size();
    for (int i = dayBuy + 1; i < len; i++)
    {
        if (price[i] - price[dayBuy] > profit)
        {
            profit = price[i] - price[dayBuy];
            luckyday = i + 1;
        }
    }
    return luckyday;
}

int main()
{
    int dayBuy = 0;
    int luckyday = -1;
    double profit = 0;
    std::string content;
    
    // Exit program if file reading fails
    if (file_to_string(content) == -1)
    {
        return 0;
    }
    
    std::vector<double> price;
    array_load_from_string(content, price);
    int len = price.size();
    
    std::cout << "Input the day you would buy the stock \n";
    while (true) {
        // Check if input is a valid integer
        if (!(std::cin >> dayBuy))
        {
            std::cerr << "Invalid input. Please enter an integer: ";
            std::cin.clear();  // Clear input error flag
            // Ignore all remaining invalid input until newline
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        // Check if input is within valid range
        else if (dayBuy > len || dayBuy < 1)
        {
            std::cerr << "Input out of range. Please enter a number between 1 and " << len << ": ";
        }
        else
        {
            break;  // Exit loop if input is valid
        }
    }
    
    dayBuy--;  // Convert 1-based buy day to 0-based index

    luckyday = findLuckyday(dayBuy, price, profit);
    
    // Output result based on whether a profitable day exists
    if (luckyday == -1) {
        std::cout << "No profitable day to sell after the chosen buy day." << std::endl;
        std::cout << "And the profit is 0" << std::endl;
    }
    else {
        std::cout << "The best day to sell is " << luckyday << std::endl;
        std::cout << "And the profit is " << profit << std::endl;
    }
    
    return 0;
}
