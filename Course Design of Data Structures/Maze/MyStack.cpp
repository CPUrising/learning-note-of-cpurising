#include "maze.h"

template <typename T>
MyStack<T>::MyStack() : topNode(nullptr), size(0) {}

template <typename T>
MyStack<T>::~MyStack() {
    while (!isEmpty()) {
        pop();
    }
}

template <typename T>
void MyStack<T>::push(const T& value) {
    Node* newNode = new Node(value);
    newNode->next = topNode;
    topNode = newNode;
    size++;
}

template <typename T>
void MyStack<T>::pop() {
    if (isEmpty()) return;
    Node* temp = topNode;
    topNode = topNode->next;
    delete temp;
    size--;
}

template <typename T>
T MyStack<T>::top() const {
    if (isEmpty()) throw "Stack is empty";
    return topNode->data;
}

template <typename T>
bool MyStack<T>::isEmpty() const {
    return topNode == nullptr;
}

template <typename T>
int MyStack<T>::getSize() const {
    return size;
}

// 显式实例化模板（针对需要使用的类型）
template class MyStack<std::pair<int, int>>;