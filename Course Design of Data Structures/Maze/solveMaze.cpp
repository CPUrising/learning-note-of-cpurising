#include "maze.h"
#include<vector>
#include <stack>
using namespace std;
int solveMaze(Maze &maze, vector<pair<int, int>> &path)
{
	pair<int, int> start = maze.getEntrance();
	pair<int, int> end = maze.getExit();
	int startX = start.first;
	int startY = start.second + 1; // 入口在左侧，内侧列号+1

	//常用构造函数 vector<T>(size_t n, const T & value); 示例：vector<int> arr(5, 0); 会创建一个包含 5 个 int 的向量，每个元素都是 0（即 [0,0,0,0,0]）。
	vector<vector<bool>> visited(maze.getRows(), vector<bool>(maze.getCols(), false));
	// 方向：上、右、下、左
	const int dx[] = { -1,0,1,0 };
	const int dy[] = { 0,1,0,-1 };

	stack <pair<int, int>> s;
	s.push(start);
	visited[startX][startY] = true;
	path.push_back(start);

	while(!s.empty())
	{
		pair<int, int> current = s.top();
		int x = current.first;
		int y = current.second;
		if (x == end.first && y == end.second)
			return true;
		
		bool found = false;
		for (int i = 0; i < 4; i++)
		{
			int nx = x + dx[i];
			int ny = y + dy[i];

			if (maze.isPath(nx, ny) && !visited[nx][ny])
			{
				found = true;
				s.push({ nx,ny });
				visited[nx][ny] = true;
				path.push_back({ nx,ny });
				break;
			}
		}
		if (!found)
		{
			s.pop();
			path.pop_back();
		}
	}
	path.clear();
	return false;
}