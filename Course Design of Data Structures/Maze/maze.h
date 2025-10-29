#pragma once
#include <vector>
#include <utility>
#include <stack>
#include <iostream>

// 迷宫单元格状态
enum CellState {
	WALL,       // 墙
	PATH,       // 通路
	ENTRANCE,   // 入口
	EXIT        // 出口
};

class Maze {
private:
	int rows;          // 迷宫行数
	int cols;          // 迷宫列数
	std::vector<std::vector<CellState>> maze;  // 迷宫数据
	std::pair<int, int> entrance;  // 入口位置
	std::pair<int, int> exit;      // 出口位置

public:
	// 构造函数：传入期望的行数和列数
	Maze(int r, int c);
	//重写一个析构函数
	~Maze() {};
	// 显示迷宫
	void display() const;

	// 获取入口位置（行, 列）
	std::pair<int, int> getEntrance() const;

	// 获取出口位置（行, 列）
	std::pair<int, int> getExit() const;

	// 检查坐标(x,y)是否为可通行路径（包括出口）
	bool isPath(int x, int y) const;

	// 获取迷宫行数
	int getRows() const { return rows; }

	// 获取迷宫列数
	int getCols() const { return cols; }

private:
	// 生成迷宫的核心函数（私有，仅内部调用）
	void generateMaze();
};
int solveMaze(Maze& maze, std::vector<std::pair<int, int>>& path);