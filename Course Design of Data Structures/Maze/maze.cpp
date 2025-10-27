#include "maze.h"
#include <iostream>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <algorithm> // 用于max函数

using namespace std;

// 方向定义(上、右、下、左)
const int dx[] = { -1, 0, 1, 0 };
const int dy[] = { 0, 1, 0, -1 };

// 构造函数实现
Maze::Maze(int r, int c) {
	// 确保行数和列数为奇数（便于生成迷宫路径）
	rows = (r % 2 == 0) ? r + 1 : r;
	cols = (c % 2 == 0) ? c + 1 : c;
	// 确保最小尺寸（避免过小迷宫无法生成）
	rows = max(5, rows);
	cols = max(5, cols);

	// 初始化迷宫为全墙
	maze.resize(rows, vector<CellState>(cols, WALL));

	srand(time(0)); // 初始化随机数种子
	generateMaze(); // 生成迷宫
}

// 生成迷宫（深度优先搜索算法）
void Maze::generateMaze() {
	// 从(1,1)开始（确保在迷宫内部）
	stack<pair<int, int>> path;
	int startX = 1, startY = 1;
	maze[startX][startY] = PATH;
	path.push({ startX, startY });

	// 设置入口（左侧边界，与起点相邻）
	entrance = { 1, 0 };
	maze[1][0] = ENTRANCE;
	maze[1][1] = PATH; // 入口内侧设为通路

	// 深度优先搜索生成路径
	while (!path.empty()) {
		auto current = path.top();
		int x = current.first;
		int y = current.second;

		// 收集未访问的邻居（间隔一个格子，确保墙的存在）
		vector<int> directions;
		for (int i = 0; i < 4; i++) {
			int nx = x + 2 * dx[i];
			int ny = y + 2 * dy[i];
			// 检查是否在迷宫内部且未访问（仍是墙）
			if (nx > 0 && nx < rows - 1 && ny > 0 && ny < cols - 1 && maze[nx][ny] == WALL) {
				directions.push_back(i);
			}
		}

		if (!directions.empty()) {
			// 随机选择一个方向
			int dir = directions[rand() % directions.size()];
			int nx = x + 2 * dx[dir];
			int ny = y + 2 * dy[dir];

			// 打通当前格子与目标格子之间的墙
			maze[x + dx[dir]][y + dy[dir]] = PATH;
			maze[nx][ny] = PATH;

			path.push({ nx, ny }); // 继续探索新格子
		}
		else {
			path.pop(); // 回溯
		}
	}

	// 设置出口（右侧边界，找最后一行的通路）
	for (int j = cols - 2; j >= 0; j--) {
		if (maze[rows - 2][j] == PATH) {
			exit = { rows - 2, cols - 1 };
			maze[rows - 2][cols - 1] = EXIT;
			break;
		}
	}
}

// 显示迷宫
void Maze::display() const {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			switch (maze[i][j]) {
			case WALL:       cout << "■"; break;
			case PATH:       cout << "  "; break;
			case ENTRANCE:   cout << "入"; break;
			case EXIT:       cout << "出"; break;
			}
		}
		cout << endl;
	}
}

// 获取入口位置
pair<int, int> Maze::getEntrance() const {
	return entrance;
}

// 获取出口位置
pair<int, int> Maze::getExit() const {
	return exit;
}

// 检查坐标是否为通路
bool Maze::isPath(int x, int y) const {
	if (x < 0 || x >= rows || y < 0 || y >= cols)
		return false;
	return maze[x][y] == PATH || maze[x][y] == EXIT;
}