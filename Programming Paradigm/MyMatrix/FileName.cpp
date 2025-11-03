class CMatrix
{
public:
    CMatrix(int Row, int Col)
        : m_iRow(Row), m_iCol(Col)
    {
        m_pdMat = new double[Row * Col];
    }

    ~CMatrix()
    {
        delete[] m_pdMat;
    }

private:
    int m_iRow, m_iCol;
    double* m_pdMat;
};
CMatrix& CMatrix::operator=(const CMatrix& rhs)
{
    if (this != &rhs)
    {
        m_iRow = rhs.m_iRow;
        m_iCol = rhs.m_iCol;
        int num = m_iRow * m_iCol;
        delete[] m_pdMat;
        m_pdMat = new double[num];
        for (int i = 0; i < num; i++)
            m_pdMat[i] = rhs.m_pdMat[i];
    }
    return *this;
}

int main()
{
    CMatrix mat(4, 4);  //constructor called
    {
        CMatrix mat2 = mat;//默认拷贝构造函数
        //default memberwise initialization used here, so mat2.m_pdMat == mat2.m_pdMat

       //end of mat2’s scope, so its destructor is called, thus the mem from heap is released
    }
    //end of mat’s scope, so its destructor is called, but what to release?
};
