#include <iostream>
#include <vector>
#include <algorithm>
#include <bitset>
using namespace std;

const int INF = -1;

// 输入验证函数：确保输入为正整数
void inputNum(int& len) {
    while (1) {
        cin >> len;
        if (!cin.good() || len <= 0) {  // 增加对非正整数的判断
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "输入无效，请重新输入正整数: ";
        }
        else {
            break;
        }
    }
}

// 边的输入验证函数
void inputEdge(int& u, int& v, int& w, int n) {
    while (1) {
        cin >> u >> v >> w;
        if (!cin.good() || u < 1 || u > n || v < 1 || v > n || w <= 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "边输入无效，请重新输入（格式：起点 终点 正权重，顶点范围1-" << n << "）: ";
        }
        else {
            break;
        }
    }
}

class ChinesePostman {
private:
    // Floyd算法求最短路径
    void floyd(vector<vector<int>>& graph, int n) {
        for (int k = 1; k <= n; ++k) {
            for (int i = 1; i <= n; ++i) {
                for (int j = 1; j <= n; ++j) {
                    if (i == j) continue;
                    if (graph[i][k] != INF && graph[k][j] != INF) {
                        if (graph[i][j] == INF) {
                            graph[i][j] = graph[i][k] + graph[k][j];
                        }
                        else {
                            graph[i][j] = min(graph[i][j], graph[i][k] + graph[k][j]);
                        }
                    }
                }
            }
        }
    }

public:
    int solve(vector<vector<int>>& graph, vector<int>& degree, int n) {
        // 计算原图总权重
        int totalWeight = 0;
        for (int i = 1; i <= n; ++i) {
            for (int j = i + 1; j <= n; ++j) {
                if (graph[i][j] != INF) {
                    totalWeight += graph[i][j];
                }
            }
        }

        // 找到所有奇度顶点
        vector<int> oddVertices;
        oddVertices.push_back(0);  // 占位使索引从1开始
        for (int i = 1; i <= n; ++i) {
            if (degree[i] % 2 != 0) {
                oddVertices.push_back(i);
            }
        }

        int m = oddVertices.size() - 1;  // 奇度顶点数量
        if (m == 0) {
            // 无奇度顶点，直接返回总权重
            return totalWeight;
        }

        // 计算所有奇度顶点之间的最短路径
        floyd(graph, n);

        // 检查是否存在无法到达的奇度顶点对
        for (int i = 1; i <= m; ++i) {
            for (int j = i + 1; j <= m; ++j) {
                if (graph[oddVertices[i]][oddVertices[j]] == INF) {
                    return INF;  // 存在无法到达的顶点，返回无效
                }
            }
        }

        // 动态规划求最小权重匹配
        int size = 1 << m;
        vector<int> dp(size, INF);
        dp[0] = 0;

        for (int mask = 0; mask < size; ++mask) {
            if (dp[mask] == INF) continue;

            // 找到第一个未匹配的顶点
            int x = 1;
            while (x <= m && (mask & (1 << (x - 1)))) {
                x++;
            }

            // 匹配另一个未匹配的顶点
            for (int y = x + 1; y <= m; ++y) {
                if (!(mask & (1 << (y - 1)))) {
                    int u = oddVertices[x];
                    int v = oddVertices[y];
                    int newMask = mask | (1 << (x - 1)) | (1 << (y - 1));

                    if (graph[u][v] != INF) {
                        int newCost = dp[mask] + graph[u][v];
                        if (dp[newMask] == INF || newCost < dp[newMask]) {
                            dp[newMask] = newCost;
                        }
                    }
                }
            }
        }

        // 总路径权重 = 原图权重 + 最小匹配权重
        return totalWeight + dp[size - 1];
    }
};

int main() {
    int n, m;
    cout << "请输入顶点数和边数: ";
    inputNum(n);  // 输入顶点数并验证
    inputNum(m);  // 输入边数并验证

    // 初始化图和度数组
    vector<vector<int>> graph(n + 1, vector<int>(n + 1, INF));
    vector<int> degree(n + 1, 0);

    cout << "请输入" << m << "条边，格式为：起点 终点 权重: " << endl;
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        inputEdge(u, v, w, n);  // 输入边并验证
        graph[u][v] = w;
        graph[v][u] = w;
        degree[u]++;
        degree[v]++;
    }

    ChinesePostman cp;
    int result = cp.solve(graph, degree, n);

    if (result < 0) {
        cout << "无法找到有效路径（存在不连通的奇度顶点）" << endl;
    }
    else {
        cout << "中国邮递员问题的最短路径总权重为: " << result << endl;
    }

    return 0;
}