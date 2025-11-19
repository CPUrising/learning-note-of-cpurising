#include <iostream>
using namespace std;

//用结构体来表示二元关系 
typedef struct
{
	char a;
	char b;
}BR;

int n, m;      //n表示集合A中的元素个数，m表示二元关系R中的元素个数 
int fun(char ch, char*& A);
//创建集合A并初始化 
void init_aggregation(char*& A)
{
	cout << "请输入集合A中的元素个数(正整数)，按回车键确认：" << endl;
	// 新增输入验证：确保n是正整数
	while (true) {
		cin >> n;
		if (cin.good() && n > 0) { // 检查输入是否为有效整数且大于0
			break;
		}
		else {
			cin.clear(); // 清除错误状态
			cin.ignore(1024, '\n'); // 忽略缓冲区中的无效输入
			cout << "输入错误！请输入正整数：" << endl;
		}
	}
	A = new char[n];
	cout << "请依次输入集合A中的";
	cout << n;     //n是集合A中的元素个数
	cout << "个元素(形如：a b c d ......这样的格式)，按回车键输入下一项：" << endl;
	for (int i = 0; i < n; i++)
	{
		// 验证元素是否为单个字符
		while (true) {
			cin >> A[i];
			if (cin.good() && isalpha(A[i]) && cin.peek() == ' ' || cin.peek() == '\n') {
				// 检查是否为字母，且下一个字符是空格或回车（确保单个字符）
				// 额外检查是否重复
				bool duplicate = false;
				for (int j = 0; j < i; j++) {
					if (A[j] == A[i]) {
						duplicate = true;
						break;
					}
				}
				if (duplicate) {
					cout << "元素重复！请重新输入第" << i + 1 << "个元素：" << endl;
					continue;
				}
				break;
			}
			else {
				cin.clear();
				cin.ignore(1024, '\n');
				cout << "输入错误！请输入单个字母作为元素：" << endl;
			}
		}
	}
}

//创建集合A的二元关系R的集合并初始化 
void init_BinaryRelation(BR*& R, char* A) 
{
	cout << "请输入二元关系R中的元素个数(正整数)，按回车键确认：" << endl;
	// 新增输入验证：确保m是正整数
	while (true) {
		cin >> m;
		if (cin.good() && m >= 0) { // 允许m=0（空关系）
			break;
		}
		else {
			cin.clear();
			cin.ignore(1024, '\n');
			cout << "输入错误！请输入非负整数：" << endl;
		}
	}
	R = new BR[m];
	cout << "请依次输入R中的";
	cout << m;   //m是R中的元素个数
	cout << "个元素，一行是一个元素\n";
	cout << "(形如：\n";
	cout << "a b\n";
	cout << "b c\n";
	cout << "c d\n";
	cout << "......\n";
	cout << "这样的格式)，按回车键输入下一项：\n";
	for (int i = 0; i < m; i++)
	{
		cout << "请输入第" << i + 1 << "个有序对（格式：a b）：" << endl;
		while (true) {
			cin >> R[i].a >> R[i].b;
			// 检查输入是否为有效字符
			if (!isalpha(R[i].a) || !isalpha(R[i].b)) {
				cin.clear();
				cin.ignore(1024, '\n');
				cout << "输入错误！请输入字母：" << endl;
				continue;
			}
			// 检查元素是否在集合A中（调用fun函数）
			int pos_a = fun(R[i].a, A);
			int pos_b = fun(R[i].b, A);
			if (pos_a == -1 || pos_b == -1) {
				cout << "错误：元素不在集合A中！请重新输入：" << endl;
				continue;
			}
			break; // 输入正确，退出循环
		}
	}
}

int fun(char ch, char*& A)
{
	for (int i = 0; i < n; i++)
	{
		if (ch == A[i])
		{
			return i;
		}
	}
	return -1;
}

//Warshall算法的核心部分 
void Warshall(char*& A, BR*& R, bool**& tR)
{
	int i, j, k;
	int x, y;

	//用关系矩阵表示二元关系R 
	for (i = 0; i < m; i++)
	{
		x = fun(R[i].a, A);
		y = fun(R[i].b, A);
		tR[x][y] = 1;
	}

	//计算传递闭包的过程 
	for (i = 0; i < n; i++)
	{ //检索列 
		for (j = 0; j < n; j++)
		{ //检索行 
			if (tR[j][i] == 1)
			{
				for (k = 0; k < n; k++)
				{
					tR[j][k] = tR[j][k] + tR[i][k];
				}
			}
		}
	}
}

//将传递闭包t(R)的关系矩阵表示转化为集合表示 
void translation_output(char*& A, bool**& tR)
{
	cout << endl;
	cout << "R的传递闭包(集合形式)为：" << endl;
	cout << "t(R) = {";
	int i, j;
	int f = 0;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (tR[i][j] == 1)
			{
				if (f == 1)
					cout << ",";
				else
					f = 1;
				cout << "<" << A[i] << "," << A[j] << ">";
			}
		}
	}
	cout << "}" << endl;
}

//主函数
int main()
{
	char* A;
	init_aggregation(A); //初始化集合A

	BR* R;
	init_BinaryRelation(R,A); //初始化二元关系

	bool** tR; //传递闭包矩阵

	//动态开辟bool类型的二维数组
	tR = new bool* [n];
	for (int i = 0; i < n; i++)
	{
		tR[i] = new bool[n];
	}

	//初始化二维数组(全部赋值为0)
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			tR[i][j] = 0;
		}
	}

	Warshall(A, R, tR);//调用Warshall算法函数

	translation_output(A, tR); //将传递闭包t(R)的关系矩阵表示转化为集合表示 

	// 补充内存释放
	delete[] A;
	delete[] R;
	for (int i = 0; i < n; i++) {
		delete[] tR[i];
	}
	delete[] tR;

	return 0;
}