#include <iostream>
using namespace std;

class Vector3D {
private:
    float x, y, z;

public:
    Vector3D() {
        cin >> x >> y >> z;
    }

    Vector3D(const float arr[3]) {
        x = arr[0];
        y = arr[1];
        z = arr[2];
    }

    Vector3D add(const Vector3D& v) const {
        float res[3] = { x + v.x, y + v.y, z + v.z };
        return Vector3D(res);
    }

    float dot(const Vector3D& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3D cross(const Vector3D& v) const {
        float res[3] = {
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        };
        return Vector3D(res);
    }

    void print() const {
        cout << "(" << x << ", " << y << ", " << z << ")" << endl;
    }

    Vector3D operator+(const Vector3D& v) const {
        return add(v);
    }

    Vector3D operator-(const Vector3D& v) const {
        float res[3] = { x - v.x, y - v.y, z - v.z };
        return Vector3D(res);
    }

    Vector3D operator*(float scalar) const {
        float res[3] = { x * scalar, y * scalar, z * scalar };
        return Vector3D(res);
    }

    Vector3D& operator+=(const Vector3D& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    float& operator[](int index) {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default:
            cerr << "索引越界！" << endl;
            exit(1);
        }
    }
};

int main() {
    cout << "请输入向量v1的三个分量：";
    Vector3D v1;

    float arr[3] = { 1.5f, 2.5f, 3.5f };
    Vector3D v2(arr);

    cout << "v1: ";
    v1.print();
    cout << "v2: ";
    v2.print();

    Vector3D v3 = v1 + v2;
    cout << "v1 + v2 = ";
    v3.print();

    Vector3D v4 = v1 - v2;
    cout << "v1 - v2 = ";
    v4.print();

    Vector3D v5 = v2 * 2.0f;
    cout << "v2 * 2 = ";
    v5.print();

    cout << "v1 · v2 = " << v1.dot(v2) << endl;

    Vector3D v6 = v1.cross(v2);
    cout << "v1 × v2 = ";
    v6.print();

    v1 += v2;
    cout << "v1 += v2 后，v1 = ";
    v1.print();

    cout << "v2[0] = " << v2[0] << ", v2[1] = " << v2[1] << ", v2[2] = " << v2[2] << endl;

    v2[0] = 10.0f;
    v2[1] = 20.0f;
    v2[2] = 30.0f;
    cout << "修改后v2: ";
    v2.print();

    return 0;
}