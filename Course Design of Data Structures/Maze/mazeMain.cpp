#include "maze.h"
using namespace std;

void inputNum(int& len)//cin and check
{
	while (1)
	{
		cin >> len;
		if (!cin.good())
		{
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "Please re-enter a positive integer:\n";
		}
		else
			break;
	}
}

int main() {
	int rows, cols;
	cout << "请输入迷宫的行数和列数（建议10-30）：";
	inputNum(rows);
	inputNum(cols);

	// 创建迷宫
	Maze maze(rows, cols);
	cout << "\n生成的迷宫：\n";
	maze.display();

	// 求解迷宫
	vector<pair<int, int>> path;
	bool success = solveMaze(maze, path);

	// 输出结果
	if (success) {
		cout << "\n找到通路！路径如下（坐标：行,列）：\n";
		for (const auto& p : path) {
			cout << "(" << p.first << "," << p.second << ") ";
		}
		cout << endl;
	}
	else {
		cout << "\n没有找到通路！\n";
	}

	return 0;
}