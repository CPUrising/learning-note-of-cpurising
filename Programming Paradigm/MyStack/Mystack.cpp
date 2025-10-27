#include <iostream>
#include <string>
using namespace std;
class StringStack {
private:
    std::string* stack;  
    int capacity;        
    int top;             

public:
    StringStack(int cap) : capacity(cap), top(-1) {
        stack = new string[capacity];
    }

    ~StringStack() {
        delete[] stack;
    }

    bool push(const string& item) {
        if (full()) {
            return false;  
        }
        stack[++top] = item;  
        return true;
    }

    string pop() {
        if (empty()) {
            return "";  
        }
        return stack[top--]; 
    }

    bool full() const {
        return top == capacity - 1;
    }

    bool empty() const {
        return top == -1;
    }

    int size() const {
        return top + 1;
    }

    string peek() const {
        if (empty()) {
            return "";
        }
        return stack[top];
    }
};