#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <algorithm>

class CMatrix {
private:
    int m_iRow, m_iCol;  // 行数、列数
    double* m_pdMat;     // 矩阵数据（行优先存储）

    // 检查索引合法性
    void checkIndex(int row, int col) const {
        if (row < 0 || row >= m_iRow || col < 0 || col >= m_iCol) {
            throw std::out_of_range("矩阵索引越界");
        }
    }

    // 检查矩阵尺寸是否相同（用于加减运算）
    void checkSize(const CMatrix& other) const {
        if (m_iRow != other.m_iRow || m_iCol != other.m_iCol) {
            throw std::invalid_argument("矩阵尺寸不匹配");
        }
    }
public:
    // 构造函数（默认、指定行列、从二维数组初始化）
    CMatrix() : m_iRow(0), m_iCol(0), m_pdMat(nullptr) {}

    CMatrix(int row, int col, double val = 0.0)
        : m_iRow(row), m_iCol(col) {
        if (row <= 0 || col <= 0) {
            throw std::invalid_argument("矩阵行列数必须为正整数");
        }
        m_pdMat = new double[row * col];
        for (int i = 0; i < row * col; ++i) {
            m_pdMat[i] = val;
        }
    }

    CMatrix(int row, int col, const double* data)
        : m_iRow(row), m_iCol(col) {
        if (row <= 0 || col <= 0 || data == nullptr) {
            throw std::invalid_argument("无效的矩阵参数");
        }
        m_pdMat = new double[row * col];
        memcpy(m_pdMat, data, row * col * sizeof(double));
    }

    // 拷贝构造函数
    CMatrix(const CMatrix& other)
        : m_iRow(other.m_iRow), m_iCol(other.m_iCol) {
        m_pdMat = new double[m_iRow * m_iCol];
        memcpy(m_pdMat, other.m_pdMat, m_iRow * m_iCol * sizeof(double));
    }

    // 移动构造函数
    CMatrix(CMatrix&& other) noexcept
        : m_iRow(other.m_iRow), m_iCol(other.m_iCol), m_pdMat(other.m_pdMat) {
        other.m_iRow = 0;
        other.m_iCol = 0;
        other.m_pdMat = nullptr;
    }

    // 析构函数
    ~CMatrix() {
        delete[] m_pdMat;
        m_pdMat = nullptr;
    }

    // 赋值运算符
    CMatrix& operator=(const CMatrix& other) {
        if (this != &other) {
            delete[] m_pdMat;
            m_iRow = other.m_iRow;
            m_iCol = other.m_iCol;
            m_pdMat = new double[m_iRow * m_iCol];
            memcpy(m_pdMat, other.m_pdMat, m_iRow * m_iCol * sizeof(double));
        }
        return *this;
    }

    // 移动赋值运算符
    CMatrix& operator=(CMatrix&& other) noexcept {
        if (this != &other) {
            delete[] m_pdMat;
            m_iRow = other.m_iRow;
            m_iCol = other.m_iCol;
            m_pdMat = other.m_pdMat;

            other.m_iRow = 0;
            other.m_iCol = 0;
            other.m_pdMat = nullptr;
        }
        return *this;
    }

    // 元素访问（带边界检查）
    double& operator()(int row, int col) {
        checkIndex(row, col);
        return m_pdMat[row * m_iCol + col];
    }

    const double& operator()(int row, int col) const {
        checkIndex(row, col);
        return m_pdMat[row * m_iCol + col];
    }

    // 矩阵运算：加法
    CMatrix operator+(const CMatrix& other) const {
        checkSize(other);
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] + other.m_pdMat[i];
        }
        return result;
    }

    // 矩阵运算：减法
    CMatrix operator-(const CMatrix& other) const {
        checkSize(other);
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] - other.m_pdMat[i];
        }
        return result;
    }

    // 矩阵运算：乘法（矩阵×矩阵）
    CMatrix operator*(const CMatrix& other) const {
        if (m_iCol != other.m_iRow) {
            throw std::invalid_argument("左矩阵列数必须等于右矩阵行数");
        }
        CMatrix result(m_iRow, other.m_iCol);
        for (int i = 0; i < m_iRow; ++i) {
            for (int j = 0; j < other.m_iCol; ++j) {
                double sum = 0.0;
                for (int k = 0; k < m_iCol; ++k) {
                    sum += (*this)(i, k) * other(k, j);
                }
                result(i, j) = sum;
            }
        }
        return result;
    }

    // 矩阵运算：数乘
    CMatrix operator*(double scalar) const {
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] * scalar;
        }
        return result;
    }

    // 友元：数乘（ scalar × 矩阵）
    friend CMatrix operator*(double scalar, const CMatrix& mat) {
        return mat * scalar;
    }

    // 矩阵转置
    CMatrix transpose() const {
        CMatrix result(m_iCol, m_iRow);
        for (int i = 0; i < m_iRow; ++i) {
            for (int j = 0; j < m_iCol; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // 求逆矩阵（仅支持方阵，使用高斯-约旦消元法）
    CMatrix inverse() const {
        if (m_iRow != m_iCol) {
            throw std::invalid_argument("仅方阵可求逆");
        }
        int n = m_iRow;
        CMatrix aug(n, 2 * n);  // 增广矩阵[A|E]

        // 初始化增广矩阵
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                aug(i, j) = (*this)(i, j);
            }
            aug(i, i + n) = 1.0;  // 单位矩阵部分
        }

        // 高斯-约旦消元
        for (int i = 0; i < n; ++i) {
            // 找主元（最大值所在行）
            int pivotRow = i;
            double maxVal = std::fabs(aug(i, i));
            for (int j = i + 1; j < n; ++j) {
                if (std::fabs(aug(j, i)) > maxVal) {
                    maxVal = std::fabs(aug(j, i));
                    pivotRow = j;
                }
            }

            // 主元为0，矩阵不可逆
            if (maxVal < 1e-10) {
                throw std::runtime_error("矩阵不可逆（行列式为0）");
            }

            // 交换当前行与主元行
            if (pivotRow != i) {
                for (int j = i; j < 2 * n; ++j) {
                    std::swap(aug(i, j), aug(pivotRow, j));
                }
            }

            // 归一化主元行
            double div = aug(i, i);
            for (int j = i; j < 2 * n; ++j) {
                aug(i, j) /= div;
            }

            // 消去其他行的第i列元素
            for (int j = 0; j < n; ++j) {
                if (j != i && std::fabs(aug(j, i)) > 1e-10) {
                    double factor = aug(j, i);
                    for (int k = i; k < 2 * n; ++k) {
                        aug(j, k) -= factor * aug(i, k);
                    }
                }
            }
        }

        // 提取逆矩阵（增广矩阵右侧部分）
        CMatrix inv(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inv(i, j) = aug(i, j + n);
            }
        }
        return inv;
    }

    // 获取行列式（仅支持方阵）
    double determinant() const {
        if (m_iRow != m_iCol) {
            throw std::invalid_argument("仅方阵有行列式");
        }
        int n = m_iRow;
        CMatrix mat(*this);  // 拷贝矩阵用于消元
        double det = 1.0;
        int sign = 1;

        for (int i = 0; i < n; ++i) {
            // 找主元
            int pivotRow = i;
            double maxVal = std::fabs(mat(i, i));
            for (int j = i + 1; j < n; ++j) {
                if (std::fabs(mat(j, i)) > maxVal) {
                    maxVal = std::fabs(mat(j, i));
                    pivotRow = j;
                }
            }

            if (maxVal < 1e-10) return 0.0;  // 行列式为0

            // 交换行时改变符号
            if (pivotRow != i) {
                std::swap_ranges(mat.m_pdMat + i * n, mat.m_pdMat + (i + 1) * n,
                    mat.m_pdMat + pivotRow * n);
                sign *= -1;
            }

            // 记录主元乘积
            det *= mat(i, i);

            // 消去下方元素
            for (int j = i + 1; j < n; ++j) {
                double factor = mat(j, i) / mat(i, i);
                for (int k = i; k < n; ++k) {
                    mat(j, k) -= factor * mat(i, k);
                }
            }
        }

        return det * sign;
    }

    // 打印矩阵
    void print(const std::string& name = "") const {
        if (!name.empty()) {
            std::cout << name << " = " << std::endl;
        }
        for (int i = 0; i < m_iRow; ++i) {
            for (int j = 0; j < m_iCol; ++j) {
                std::cout << std::fixed;
                std::cout.width(10);
                std::cout.precision(4);
                std::cout << (*this)(i, j);
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // 获取行列数
    int rows() const { return m_iRow; }
    int cols() const { return m_iCol; }


};

int main() {
    try {
        // 1. 矩阵创建与初始化
        std::cout << "=== 1. 矩阵创建示例 ===" << std::endl;

        // 创建3x3全0矩阵
        CMatrix mat1(3, 3);
        mat1.print("3x3全0矩阵");

        // 创建2x2矩阵并初始化值为5.0
        CMatrix mat2(2, 2, 5.0);
        mat2.print("2x2全5矩阵");

        // 从数组初始化矩阵
        double data[] = { 1, 2, 3, 4, 5, 6 };
        CMatrix mat3(2, 3, data);  // 2行3列矩阵
        mat3.print("从数组初始化的2x3矩阵");

        // 2. 矩阵元素访问与修改
        std::cout << "=== 2. 元素访问示例 ===" << std::endl;
        mat3(1, 2) = 100;  // 修改第2行第3列元素（索引从0开始）
        mat3.print("修改元素后的mat3");
        std::cout << "mat3(1,2) 的值: " << mat3(1, 2) << std::endl << std::endl;

        // 3. 矩阵基本运算（加减乘）
        std::cout << "=== 3. 矩阵运算示例 ===" << std::endl;

        // 矩阵加法
        double dataA[] = { 1, 2, 3, 4 };
        double dataB[] = { 5, 6, 7, 8 };
        CMatrix A(2, 2, dataA);
        CMatrix B(2, 2, dataB);
        A.print("A");
        B.print("B");

        CMatrix C = A + B;
        C.print("A + B");

        // 矩阵减法
        CMatrix D = B - A;
        D.print("B - A");

        // 矩阵乘法（矩阵×矩阵）
        CMatrix E = A * B;
        E.print("A × B");

        // 数乘（标量×矩阵）
        CMatrix F = 2 * A;
        F.print("2 × A");

        // 4. 矩阵转置
        std::cout << "=== 4. 矩阵转置示例 ===" << std::endl;
        CMatrix mat4(3, 2);  // 3行2列矩阵
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                mat4(i, j) = i * 2 + j + 1;  // 初始化值: 1,2; 3,4; 5,6
            }
        }
        mat4.print("原始3x2矩阵");

        CMatrix mat4T = mat4.transpose();  // 转置为2x3矩阵
        mat4T.print("转置后的2x3矩阵");

        // 5. 行列式与逆矩阵（仅方阵）
        std::cout << "=== 5. 行列式与逆矩阵示例 ===" << std::endl;
        double dataG[] = { 4, 7, 2, 6 };  // 2x2可逆矩阵
        CMatrix G(2, 2, dataG);
        G.print("矩阵G");

        // 计算行列式
        double detG = G.determinant();
        std::cout << "G的行列式值: " << detG << std::endl << std::endl;

        // 求逆矩阵
        CMatrix G_inv = G.inverse();
        G_inv.print("G的逆矩阵");

        // 验证：原矩阵 × 逆矩阵 应接近单位矩阵
        CMatrix G_check = G * G_inv;
        G_check.print("G × G逆（应接近单位矩阵）");

        // 6. 异常处理示例（故意触发错误）
        std::cout << "=== 6. 异常处理示例 ===" << std::endl;
        try {
            // 尝试创建行数为0的矩阵（非法）
            CMatrix errMat(0, 3);
        }
        catch (const std::exception& e) {
            std::cerr << "捕获错误: " << e.what() << std::endl;
        }

        try {
            // 尝试用不同尺寸矩阵相加（非法）
            CMatrix mat5(2, 3);
            CMatrix mat6(3, 2);
            CMatrix err = mat5 + mat6;
        }
        catch (const std::exception& e) {
            std::cerr << "捕获错误: " << e.what() << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "程序错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}