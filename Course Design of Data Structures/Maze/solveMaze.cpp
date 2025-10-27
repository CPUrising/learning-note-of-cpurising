#include "maze.h"
#include<vector>
#include <stack>
using namespace std;
int solveMaze(Maze &maze, vector<pair<int, int>> &path)
{
	pair<int, int> start = maze.getEntrance();
	pair<int, int> end = maze.getExit();
	int startX = start.first;
	int startY = start.second + 1; // �������࣬�ڲ��к�+1

	//���ù��캯�� vector<T>(size_t n, const T & value); ʾ����vector<int> arr(5, 0); �ᴴ��һ������ 5 �� int ��������ÿ��Ԫ�ض��� 0���� [0,0,0,0,0]����
	vector<vector<bool>> visited(maze.getRows(), vector<bool>(maze.getCols(), false));
	// �����ϡ��ҡ��¡���
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