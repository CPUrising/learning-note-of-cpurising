#include "maze.h"
#include <iostream>
#include <vector>
#include <stack>
#include "solveMaze.cpp"
using namespace std;

int main() {
	int rows, cols;
	cout << "�������Թ�������������������10-30����";
	cin >> rows >> cols;

	// �����Թ�
	Maze maze(rows, cols);
	cout << "\n���ɵ��Թ���\n";
	maze.display();

	// ����Թ�
	vector<pair<int, int>> path;
	bool success = solveMaze(maze, path);

	// ������
	if (success) {
		cout << "\n�ҵ�ͨ·��·�����£����꣺��,�У���\n";
		for (const auto& p : path) {
			cout << "(" << p.first << "," << p.second << ") ";
		}
		cout << endl;
	}
	else {
		cout << "\nû���ҵ�ͨ·��\n";
	}

	return 0;
}