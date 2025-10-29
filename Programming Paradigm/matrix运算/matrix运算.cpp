#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <algorithm>

class CMatrix {
public:
    // ���캯����Ĭ�ϡ�ָ�����С��Ӷ�ά�����ʼ����
    CMatrix() : m_iRow(0), m_iCol(0), m_pdMat(nullptr) {}

    CMatrix(int row, int col, double val = 0.0)
        : m_iRow(row), m_iCol(col) {
        if (row <= 0 || col <= 0) {
            throw std::invalid_argument("��������������Ϊ������");
        }
        m_pdMat = new double[row * col];
        for (int i = 0; i < row * col; ++i) {
            m_pdMat[i] = val;
        }
    }

    CMatrix(int row, int col, const double* data)
        : m_iRow(row), m_iCol(col) {
        if (row <= 0 || col <= 0 || data == nullptr) {
            throw std::invalid_argument("��Ч�ľ������");
        }
        m_pdMat = new double[row * col];
        memcpy(m_pdMat, data, row * col * sizeof(double));
    }

    // �������캯��
    CMatrix(const CMatrix& other)
        : m_iRow(other.m_iRow), m_iCol(other.m_iCol) {
        m_pdMat = new double[m_iRow * m_iCol];
        memcpy(m_pdMat, other.m_pdMat, m_iRow * m_iCol * sizeof(double));
    }

    // �ƶ����캯��
    CMatrix(CMatrix&& other) noexcept
        : m_iRow(other.m_iRow), m_iCol(other.m_iCol), m_pdMat(other.m_pdMat) {
        other.m_iRow = 0;
        other.m_iCol = 0;
        other.m_pdMat = nullptr;
    }

    // ��������
    ~CMatrix() {
        delete[] m_pdMat;
        m_pdMat = nullptr;
    }

    // ��ֵ�����
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

    // �ƶ���ֵ�����
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

    // Ԫ�ط��ʣ����߽��飩
    double& operator()(int row, int col) {
        checkIndex(row, col);
        return m_pdMat[row * m_iCol + col];
    }

    const double& operator()(int row, int col) const {
        checkIndex(row, col);
        return m_pdMat[row * m_iCol + col];
    }

    // �������㣺�ӷ�
    CMatrix operator+(const CMatrix& other) const {
        checkSize(other);
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] + other.m_pdMat[i];
        }
        return result;
    }

    // �������㣺����
    CMatrix operator-(const CMatrix& other) const {
        checkSize(other);
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] - other.m_pdMat[i];
        }
        return result;
    }

    // �������㣺�˷������������
    CMatrix operator*(const CMatrix& other) const {
        if (m_iCol != other.m_iRow) {
            throw std::invalid_argument("�����������������Ҿ�������");
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

    // �������㣺����
    CMatrix operator*(double scalar) const {
        CMatrix result(m_iRow, m_iCol);
        for (int i = 0; i < m_iRow * m_iCol; ++i) {
            result.m_pdMat[i] = m_pdMat[i] * scalar;
        }
        return result;
    }

    // ��Ԫ�����ˣ� scalar �� ����
    friend CMatrix operator*(double scalar, const CMatrix& mat) {
        return mat * scalar;
    }

    // ����ת��
    CMatrix transpose() const {
        CMatrix result(m_iCol, m_iRow);
        for (int i = 0; i < m_iRow; ++i) {
            for (int j = 0; j < m_iCol; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // ������󣨽�֧�ַ���ʹ�ø�˹-Լ����Ԫ����
    CMatrix inverse() const {
        if (m_iRow != m_iCol) {
            throw std::invalid_argument("�����������");
        }
        int n = m_iRow;
        CMatrix aug(n, 2 * n);  // �������[A|E]

        // ��ʼ���������
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                aug(i, j) = (*this)(i, j);
            }
            aug(i, i + n) = 1.0;  // ��λ���󲿷�
        }

        // ��˹-Լ����Ԫ
        for (int i = 0; i < n; ++i) {
            // ����Ԫ�����ֵ�����У�
            int pivotRow = i;
            double maxVal = std::fabs(aug(i, i));
            for (int j = i + 1; j < n; ++j) {
                if (std::fabs(aug(j, i)) > maxVal) {
                    maxVal = std::fabs(aug(j, i));
                    pivotRow = j;
                }
            }

            // ��ԪΪ0�����󲻿���
            if (maxVal < 1e-10) {
                throw std::runtime_error("���󲻿��棨����ʽΪ0��");
            }

            // ������ǰ������Ԫ��
            if (pivotRow != i) {
                for (int j = i; j < 2 * n; ++j) {
                    std::swap(aug(i, j), aug(pivotRow, j));
                }
            }

            // ��һ����Ԫ��
            double div = aug(i, i);
            for (int j = i; j < 2 * n; ++j) {
                aug(i, j) /= div;
            }

            // ��ȥ�����еĵ�i��Ԫ��
            for (int j = 0; j < n; ++j) {
                if (j != i && std::fabs(aug(j, i)) > 1e-10) {
                    double factor = aug(j, i);
                    for (int k = i; k < 2 * n; ++k) {
                        aug(j, k) -= factor * aug(i, k);
                    }
                }
            }
        }

        // ��ȡ�������������Ҳಿ�֣�
        CMatrix inv(n, n);
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                inv(i, j) = aug(i, j + n);
            }
        }
        return inv;
    }

    // ��ȡ����ʽ����֧�ַ���
    double determinant() const {
        if (m_iRow != m_iCol) {
            throw std::invalid_argument("������������ʽ");
        }
        int n = m_iRow;
        CMatrix mat(*this);  // ��������������Ԫ
        double det = 1.0;
        int sign = 1;

        for (int i = 0; i < n; ++i) {
            // ����Ԫ
            int pivotRow = i;
            double maxVal = std::fabs(mat(i, i));
            for (int j = i + 1; j < n; ++j) {
                if (std::fabs(mat(j, i)) > maxVal) {
                    maxVal = std::fabs(mat(j, i));
                    pivotRow = j;
                }
            }

            if (maxVal < 1e-10) return 0.0;  // ����ʽΪ0

            // ������ʱ�ı����
            if (pivotRow != i) {
                std::swap_ranges(mat.m_pdMat + i * n, mat.m_pdMat + (i + 1) * n,
                    mat.m_pdMat + pivotRow * n);
                sign *= -1;
            }

            // ��¼��Ԫ�˻�
            det *= mat(i, i);

            // ��ȥ�·�Ԫ��
            for (int j = i + 1; j < n; ++j) {
                double factor = mat(j, i) / mat(i, i);
                for (int k = i; k < n; ++k) {
                    mat(j, k) -= factor * mat(i, k);
                }
            }
        }

        return det * sign;
    }

    // ��ӡ����
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

    // ��ȡ������
    int rows() const { return m_iRow; }
    int cols() const { return m_iCol; }

private:
    int m_iRow, m_iCol;  // ����������
    double* m_pdMat;     // �������ݣ������ȴ洢��

    // ��������Ϸ���
    void checkIndex(int row, int col) const {
        if (row < 0 || row >= m_iRow || col < 0 || col >= m_iCol) {
            throw std::out_of_range("��������Խ��");
        }
    }

    // ������ߴ��Ƿ���ͬ�����ڼӼ����㣩
    void checkSize(const CMatrix& other) const {
        if (m_iRow != other.m_iRow || m_iCol != other.m_iCol) {
            throw std::invalid_argument("����ߴ粻ƥ��");
        }
    }
};

int main() {
    try {
        // 1. ���󴴽����ʼ��
        std::cout << "=== 1. ���󴴽�ʾ�� ===" << std::endl;

        // ����3x3ȫ0����
        CMatrix mat1(3, 3);
        mat1.print("3x3ȫ0����");

        // ����2x2���󲢳�ʼ��ֵΪ5.0
        CMatrix mat2(2, 2, 5.0);
        mat2.print("2x2ȫ5����");

        // �������ʼ������
        double data[] = { 1, 2, 3, 4, 5, 6 };
        CMatrix mat3(2, 3, data);  // 2��3�о���
        mat3.print("�������ʼ����2x3����");

        // 2. ����Ԫ�ط������޸�
        std::cout << "=== 2. Ԫ�ط���ʾ�� ===" << std::endl;
        mat3(1, 2) = 100;  // �޸ĵ�2�е�3��Ԫ�أ�������0��ʼ��
        mat3.print("�޸�Ԫ�غ��mat3");
        std::cout << "mat3(1,2) ��ֵ: " << mat3(1, 2) << std::endl << std::endl;

        // 3. ����������㣨�Ӽ��ˣ�
        std::cout << "=== 3. ��������ʾ�� ===" << std::endl;

        // ����ӷ�
        double dataA[] = { 1, 2, 3, 4 };
        double dataB[] = { 5, 6, 7, 8 };
        CMatrix A(2, 2, dataA);
        CMatrix B(2, 2, dataB);
        A.print("A");
        B.print("B");

        CMatrix C = A + B;
        C.print("A + B");

        // �������
        CMatrix D = B - A;
        D.print("B - A");

        // ����˷������������
        CMatrix E = A * B;
        E.print("A �� B");

        // ���ˣ�����������
        CMatrix F = 2 * A;
        F.print("2 �� A");

        // 4. ����ת��
        std::cout << "=== 4. ����ת��ʾ�� ===" << std::endl;
        CMatrix mat4(3, 2);  // 3��2�о���
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 2; ++j) {
                mat4(i, j) = i * 2 + j + 1;  // ��ʼ��ֵ: 1,2; 3,4; 5,6
            }
        }
        mat4.print("ԭʼ3x2����");

        CMatrix mat4T = mat4.transpose();  // ת��Ϊ2x3����
        mat4T.print("ת�ú��2x3����");

        // 5. ����ʽ������󣨽�����
        std::cout << "=== 5. ����ʽ�������ʾ�� ===" << std::endl;
        double dataG[] = { 4, 7, 2, 6 };  // 2x2�������
        CMatrix G(2, 2, dataG);
        G.print("����G");

        // ��������ʽ
        double detG = G.determinant();
        std::cout << "G������ʽֵ: " << detG << std::endl << std::endl;

        // �������
        CMatrix G_inv = G.inverse();
        G_inv.print("G�������");

        // ��֤��ԭ���� �� ����� Ӧ�ӽ���λ����
        CMatrix G_check = G * G_inv;
        G_check.print("G �� G�棨Ӧ�ӽ���λ����");

        // 6. �쳣����ʾ�������ⴥ������
        std::cout << "=== 6. �쳣����ʾ�� ===" << std::endl;
        try {
            // ���Դ�������Ϊ0�ľ��󣨷Ƿ���
            CMatrix errMat(0, 3);
        }
        catch (const std::exception& e) {
            std::cerr << "�������: " << e.what() << std::endl;
        }

        try {
            // �����ò�ͬ�ߴ������ӣ��Ƿ���
            CMatrix mat5(2, 3);
            CMatrix mat6(3, 2);
            CMatrix err = mat5 + mat6;
        }
        catch (const std::exception& e) {
            std::cerr << "�������: " << e.what() << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "�������: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}