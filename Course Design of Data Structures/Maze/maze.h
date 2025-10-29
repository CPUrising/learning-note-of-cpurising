#pragma once
#include <vector>
#include <utility>
#include <stack>
#include <iostream>

// �Թ���Ԫ��״̬
enum CellState {
	WALL,       // ǽ
	PATH,       // ͨ·
	ENTRANCE,   // ���
	EXIT        // ����
};

class Maze {
private:
	int rows;          // �Թ�����
	int cols;          // �Թ�����
	std::vector<std::vector<CellState>> maze;  // �Թ�����
	std::pair<int, int> entrance;  // ���λ��
	std::pair<int, int> exit;      // ����λ��

public:
	// ���캯������������������������
	Maze(int r, int c);
	//��дһ����������
	~Maze() {};
	// ��ʾ�Թ�
	void display() const;

	// ��ȡ���λ�ã���, �У�
	std::pair<int, int> getEntrance() const;

	// ��ȡ����λ�ã���, �У�
	std::pair<int, int> getExit() const;

	// �������(x,y)�Ƿ�Ϊ��ͨ��·�����������ڣ�
	bool isPath(int x, int y) const;

	// ��ȡ�Թ�����
	int getRows() const { return rows; }

	// ��ȡ�Թ�����
	int getCols() const { return cols; }

private:
	// �����Թ��ĺ��ĺ�����˽�У����ڲ����ã�
	void generateMaze();
};
int solveMaze(Maze& maze, std::vector<std::pair<int, int>>& path);