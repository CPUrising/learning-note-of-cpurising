#include <iostream>
#include <windows.h>
using namespace std;

int main()
{
    int a[4];
    int p, q;
    char continueFlag;
    bool isRunning = true;

    while (isRunning)
    {
        // ��ʾ����
        cout << "***************************************\n";
        cout << "**                                   **\n";
        cout << "**        ��ӭ�����߼��������       **\n";
        cout << "**                                   **\n";
        cout << "***************************************\n\n";

        // ����P��ֵ����������
        while (true)
        {
            cout << "  ������P��ֵ��0��1��,�Իس�����: ";
            if (cin >> p)
            {
                if (p == 0 || p == 1)
                {
                    break; // ������ȷ���˳�ѭ��
                }
                else
                {
                    cout << "  P��ֵ��������,������0��1!\n";
                }
            }
            else
            {
                cout << "  �����ʽ����,����������0��1!\n";
                cin.clear(); // �������״̬
                cin.ignore(1000, '\n'); // ���Ի������еĴ�������
            }
        }

        // ����Q��ֵ����������
        while (true)
        {
            cout << "  ������Q��ֵ��0��1��,�Իس�����: ";
            if (cin >> q)
            {
                if (q == 0 || q == 1)
                {
                    break; // ������ȷ���˳�ѭ��
                }
                else
                {
                    cout << "  Q��ֵ��������,������0��1!\n";
                }
            }
            else
            {
                cout << "  �����ʽ����,����������0��1!\n";
                cin.clear(); // �������״̬
                cin.ignore(1000, '\n'); // ���Ի������еĴ�������
            }
        }

        // �߼�����
        a[0] = p && q;                // ������
        a[1] = p || q;                // ������
        a[2] = (!p) || q;             // �̺�����
        a[3] = ((!p) || q) && ((!q) || p);  // ��ֵ����

        // ������
        cout << "\n\n  ��ȡ:\n       P/\\Q = " << a[0] << "\n";
        cout << "  ��ȡ:\n       P\\/Q = " << a[1] << "\n";
        cout << "  ����:\n       P->Q = " << a[2] << "\n";
        cout << "  ˫����:\n       P<->Q = " << a[3] << "\n";

        // ѯ���Ƿ��������������
        while (true)
        {
            cout << "\n�Ƿ��������?��y/n��";
            cin >> continueFlag;

            if (continueFlag == 'y' || continueFlag == 'Y')
            {
                system("cls"); // ����
                break;
            }
            else if (continueFlag == 'n' || continueFlag == 'N')
            {
                cout << "��ӭ�´��ٴ�ʹ��!\n";
                isRunning = false;
                break;
            }
            else
            {
                cout << "�������,����������(y��n)!\n";
                cin.clear();
                cin.ignore(1000, '\n');
            }
        }
    }

    return 0;
}
