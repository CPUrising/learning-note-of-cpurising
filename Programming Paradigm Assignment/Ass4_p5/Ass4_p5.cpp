//题目 5
//
//设计一个基类`Shape`（形状），包含一个虚函数`clone()`，该函数创建并返回当前形状的副本。派生两个子类`Circle`（圆形）和`Rectangle`（矩形），每个子类均重写`clone()`函数，返回自身类型的副本。
//
//在`main()`函数中：
//
//* 创建一个存储`Shape*`指针的向量，包含一个`Circle`对象和一个`Rectangle`对象。
//
//* 遍历该向量，对每个元素调用`clone()`函数。
//
//* 使用`typeid`或`dynamic_cast`验证每个克隆对象是否保留了原始类型。
//
//
//之后，修改`clone()`函数，使其返回`unique_ptr<Shape > `而非原始指针。讨论在此场景下使用智能指针的优势。

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