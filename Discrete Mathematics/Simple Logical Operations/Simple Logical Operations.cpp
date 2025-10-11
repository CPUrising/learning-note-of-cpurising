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
        // 显示标题
        cout << "***************************************\n";
        cout << "**                                   **\n";
        cout << "**        欢迎进入逻辑运算程序       **\n";
        cout << "**                                   **\n";
        cout << "***************************************\n\n";

        // 输入P的值，带错误处理
        while (true)
        {
            cout << "  请输入P的值（0或1）,以回车结束: ";
            if (cin >> p)
            {
                if (p == 0 || p == 1)
                {
                    break; // 输入正确，退出循环
                }
                else
                {
                    cout << "  P的值输入有误,请输入0或1!\n";
                }
            }
            else
            {
                cout << "  输入格式错误,请输入整数0或1!\n";
                cin.clear(); // 清除错误状态
                cin.ignore(1000, '\n'); // 忽略缓冲区中的错误输入
            }
        }

        // 输入Q的值，带错误处理
        while (true)
        {
            cout << "  请输入Q的值（0或1）,以回车结束: ";
            if (cin >> q)
            {
                if (q == 0 || q == 1)
                {
                    break; // 输入正确，退出循环
                }
                else
                {
                    cout << "  Q的值输入有误,请输入0或1!\n";
                }
            }
            else
            {
                cout << "  输入格式错误,请输入整数0或1!\n";
                cin.clear(); // 清除错误状态
                cin.ignore(1000, '\n'); // 忽略缓冲区中的错误输入
            }
        }

        // 逻辑运算
        a[0] = p && q;                // 与运算
        a[1] = p || q;                // 或运算
        a[2] = (!p) || q;             // 蕴含运算
        a[3] = ((!p) || q) && ((!q) || p);  // 等值运算

        // 输出结果
        cout << "\n\n  合取:\n       P/\\Q = " << a[0] << "\n";
        cout << "  析取:\n       P\\/Q = " << a[1] << "\n";
        cout << "  条件:\n       P->Q = " << a[2] << "\n";
        cout << "  双条件:\n       P<->Q = " << a[3] << "\n";

        // 询问是否继续，带错误处理
        while (true)
        {
            cout << "\n是否继续运算?（y/n）";
            cin >> continueFlag;

            if (continueFlag == 'y' || continueFlag == 'Y')
            {
                system("cls"); // 清屏
                break;
            }
            else if (continueFlag == 'n' || continueFlag == 'N')
            {
                cout << "欢迎下次再次使用!\n";
                isRunning = false;
                break;
            }
            else
            {
                cout << "输入错误,请重新输入(y或n)!\n";
                cin.clear();
                cin.ignore(1000, '\n');
            }
        }
    }

    return 0;
}
