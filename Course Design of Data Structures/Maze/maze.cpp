#include "maze.h"
#include <iostream>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm> // ����max����

using namespace std;

// ������(�ϡ��ҡ��¡���)
const int dx[] = { -1, 0, 1, 0 };
const int dy[] = { 0, 1, 0, -1 };

// ���캯��ʵ��
Maze::Maze(int r, int c) {
	// ȷ������������Ϊ���������������Թ�·����
	rows = (r % 2 == 0) ? r + 1 : r;
	cols = (c % 2 == 0) ? c + 1 : c;
	// ȷ����С�ߴ磨�����С�Թ��޷����ɣ�
	rows = max(5, rows);
	cols = max(5, cols);

	// ��ʼ���Թ�Ϊȫǽ
	maze.resize(rows, vector<CellState>(cols, WALL));

	srand(time(0)); // ��ʼ�����������
	generateMaze(); // �����Թ�
}

// �����Թ���������������㷨��
void Maze::generateMaze() {
	// ��(1,1)��ʼ��ȷ�����Թ��ڲ���
	stack<pair<int, int>> path;
	int startX = 1, startY = 1;
	maze[startX][startY] = PATH;
	path.push({ startX, startY });

	// ������ڣ����߽磬��������ڣ�
	entrance = { 1, 0 };
	maze[1][0] = ENTRANCE;
	maze[1][1] = PATH; // ����ڲ���Ϊͨ·

	// ���������������·��
	while (!path.empty()) {
		auto current = path.top();
		int x = current.first;
		int y = current.second;

		// �ռ�δ���ʵ��ھӣ����һ�����ӣ�ȷ��ǽ�Ĵ��ڣ�
		vector<int> directions;
		for (int i = 0; i < 4; i++) {
			int nx = x + 2 * dx[i];
			int ny = y + 2 * dy[i];
			// ����Ƿ����Թ��ڲ���δ���ʣ�����ǽ��
			if (nx > 0 && nx < rows - 1 && ny > 0 && ny < cols - 1 && maze[nx][ny] == WALL) {
				directions.push_back(i);
			}
		}

		if (!directions.empty()) {
			// ���ѡ��һ������
			int dir = directions[rand() % directions.size()];
			int nx = x + 2 * dx[dir];
			int ny = y + 2 * dy[dir];

			// ��ͨ��ǰ������Ŀ�����֮���ǽ
			maze[x + dx[dir]][y + dy[dir]] = PATH;
			maze[nx][ny] = PATH;

			path.push({ nx, ny }); // ����̽���¸���
		}
		else {
			path.pop(); // ����
		}
	}

	// ���ó��ڣ��Ҳ�߽磬�����һ�е�ͨ·��
	for (int j = cols - 2; j >= 0; j--) {
		if (maze[rows - 2][j] == PATH) {
			exit = { rows - 2, cols - 1 };
			maze[rows - 2][cols - 1] = EXIT;
			break;
		}
	}
}

// ��ʾ�Թ�
void Maze::display() const {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			switch (maze[i][j]) {
			case WALL:       cout << "��"; break;
			case PATH:       cout << "  "; break;
			case ENTRANCE:   cout << "��"; break;
			case EXIT:       cout << "��"; break;
			}
		}
		cout << endl;
	}
}

// ��ȡ���λ��
pair<int, int> Maze::getEntrance() const {
	return entrance;
}

// ��ȡ����λ��
pair<int, int> Maze::getExit() const {
	return exit;
}

// ��������Ƿ�Ϊͨ·
bool Maze::isPath(int x, int y) const {
	if (x < 0 || x >= rows || y < 0 || y >= cols)
		return false;
	return maze[x][y] == PATH || maze[x][y] == EXIT;
}