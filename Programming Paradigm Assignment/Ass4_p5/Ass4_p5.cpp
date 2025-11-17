#include <iostream>
#include <vector>
#include <memory>
#include <typeinfo>

using namespace std;

class Shape {
public:
    virtual ~Shape() = default;
    virtual unique_ptr<Shape> clone() const = 0;
};

class Circle : public Shape {
public:
    unique_ptr<Shape> clone() const override {
        return make_unique<Circle>(*this);
    }
};

class Rectangle : public Shape {
public:
    unique_ptr<Shape> clone() const override {
        return make_unique<Rectangle>(*this);
    }
};

int main() {
    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>());
    shapes.push_back(make_unique<Rectangle>());

    vector<unique_ptr<Shape>> clones;
    for (const auto& shape : shapes) {
        clones.push_back(shape->clone());
    }

    for (size_t i = 0; i < shapes.size(); ++i) {
        const auto& original = shapes[i];
        const auto& clone = clones[i];

        if (typeid(*original) == typeid(*clone)) {
            cout << "克隆对象 " << i << " 类型与原始对象一致: "
                << typeid(*original).name() << endl;
        }
        else {
            cout << "克隆对象 " << i << " 类型与原始对象不一致" << endl;
        }
    }

    return 0;
}