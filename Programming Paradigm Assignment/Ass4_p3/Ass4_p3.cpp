#include <iostream>
#include <string>
using namespace std;

class Author {
private:
    string name;
public:
    Author(const string& n) : name(n) {}
    Author(const Author& other) : name(other.name) {}
    Author& operator=(const Author& other) {
        if (this != &other) {
            name = other.name;
        }
        return *this;
    }
    string getName() const { return name; }
};

class Publisher {
private:
    string name;
public:
    Publisher(const string& n) : name(n) {}
    Publisher(const Publisher& other) : name(other.name) {}
    Publisher& operator=(const Publisher& other) {
        if (this != &other) {
            name = other.name;
        }
        return *this;
    }
    string getName() const { return name; }
};

class Book : public Author {
private:
    Publisher publisher;
public:
    Book() : Author(""), publisher("") {}
    Book(const string& authName, const string& pubName) : Author(authName), publisher(pubName) {}
    Book(const Book& other) : Author(other), publisher(other.publisher) {}
    Book& operator=(const Book& other) {
        if (this != &other) {
            Author::operator=(other);
            publisher = other.publisher;
        }
        return *this;
    }
    string getPublisherName() const { return publisher.getName(); }
};

int main() {
    Author auth("鲁迅");
    Publisher pub("人民文学出版社");
    cout << "作者: " << auth.getName() << endl;
    cout << "出版商: " << pub.getName() << endl;

    Book book1("曹雪芹", "中华书局");
    cout << "\nbook1 - 作者: " << book1.getName() << ", 出版商: " << book1.getPublisherName() << endl;

    Book book2(book1);
    cout << "book2(拷贝book1) - 作者: " << book2.getName() << ", 出版商: " << book2.getPublisherName() << endl;

    Book book3;
    book3 = book1;
    cout << "book3(赋值book1) - 作者: " << book3.getName() << ", 出版商: " << book3.getPublisherName() << endl;

    Book book4;
    cout << "book4(默认构造) - 作者: " << book4.getName() << ", 出版商: " << book4.getPublisherName() << endl;

    return 0;
}