#include<iostream>
#include<vector>
using namespace std;

vector<int>* fibon_seq(int iLength)
{
    // ��������������Ϣ�﷨
    if (iLength <= 0 || iLength >= 1024)
    {
        cerr << "Length " << iLength
            << " not supported, reset to 8"
            << endl;
        iLength = 8;
    }

    // ��Ӧ��ʹ��static�������ε��û�������
    vector<int>* Elems = new vector<int>(iLength);

    for (int iX = 0; iX < iLength; iX++)
    {
        if (iX == 0 || iX == 1)
            (*Elems)[iX] = 1;
        else
            (*Elems)[iX] = (*Elems)[iX - 1] + (*Elems)[iX - 2];
    }
    return Elems;
}

int main()
{
    // ���ú�����ȡ쳲���������
    vector<int>* fibSeq = fibon_seq(10);

    // �������Ԫ�أ�vector����ֱ����cout�������Ҫ������
    for (int num : *fibSeq)
    {
        cout << num << " ";
    }
    cout << endl;

    // �ͷŶ�̬������ڴ�
    delete fibSeq;
    return 0;
}