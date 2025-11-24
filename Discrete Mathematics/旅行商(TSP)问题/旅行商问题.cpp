#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <cstring>
using namespace std;

const int MAXN = 20;  // 最大城市数（2^20=1e6，适合DP状态压缩）
const int INF = INT_MAX / 2;  // 避免溢出

// 输入验证：确保输入为正整数
void inputNum(int& num) {
    while (true) {
        cin >> num;
        if (!cin.good() || num <= 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "输入无效！请重新输入正整数：";
        }
        else {
            break;
        }
    }
}

// 输入验证：确保城市编号合法、距离为正
void inputDist(int& u, int& v, int& d, int n) {
    while (true) {
        cin >> u >> v >> d;
        if (!cin.good() || u < 0 || u >= n || v < 0 || v >= n || u == v || d <= 0) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "输入无效！格式：城市u 城市v 距离d（u/v范围0~" << n - 1 << "，u≠v，d>0）：";
        }
        else {
            break;
        }
    }
}

// TSP核心：动态规划状态压缩法
int tspDP(const vector<vector<int>>& dist, int n) {
    // dp[mask][u]：访问集合mask，当前在u的最短路径长度
    vector<vector<int>> dp(1 << n, vector<int>(n, INF));

    // 初始化：仅访问单个城市u，路径长度为0
    for (int u = 0; u < n; ++u) {
        dp[1 << u][u] = 0;
    }

    // 遍历所有状态（mask从1到(1<<n)-1）
    for (int mask = 1; mask < (1 << n); ++mask) {
        // 遍历当前所在城市u
        for (int u = 0; u < n; ++u) {
            if (!(mask & (1 << u))) continue;  // u不在mask中，跳过
            if (dp[mask][u] == INF) continue;  // 该状态不可达，跳过

            // 尝试访问未访问的城市v
            for (int v = 0; v < n; ++v) {
                if (mask & (1 << v)) continue;  // v已访问，跳过
                // 新状态：mask | (1<<v)（添加v），当前在v
                int new_mask = mask | (1 << v);
                dp[new_mask][v] = min(dp[new_mask][v], dp[mask][u] + dist[u][v]);
            }
        }
    }

    // 最终结果：访问所有城市后，从任意u回到起点0的最短回路
    int min_cycle = INF;
    int full_mask = (1 << n) - 1;  // 所有城市都访问过的状态
    for (int u = 0; u < n; ++u) {
        min_cycle = min(min_cycle, dp[full_mask][u] + dist[u][0]);
    }

    return min_cycle;
}

int main() {
    int n, m;  // n：城市数，m：已知直接距离的城市对数量
    cout << "===== 旅行商问题（TSP）求解 =====" << endl;
    cout << "请输入城市数量（建议≤20）：";
    inputNum(n);

    // 初始化距离矩阵：dist[u][v]表示u到v的直接距离，初始为INF（不可达）
    vector<vector<int>> dist(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;  // 自身到自身距离为0
    }

    cout << "请输入直接相连的城市对数量：";
    inputNum(m);

    cout << "请输入" << m << "条城市对的距离（格式：城市u 城市v 距离d）：" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v, d;
        inputDist(u, v, d, n);
        dist[u][v] = d;
        dist[v][u] = d;  // 假设为无向图（城市间往返距离相同）
    }

    // 第一步：Floyd-Warshall算法预处理所有城市对的最短路径（处理间接可达情况）
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }

    // 检查是否存在不可达的城市对（TSP要求所有城市连通）
    bool is_connected = true;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (dist[i][j] == INF) {
                is_connected = false;
                break;
            }
        }
        if (!is_connected) break;
    }

    if (!is_connected) {
        cout << "错误：存在不可达的城市对，无法形成TSP回路！" << endl;
        return 1;
    }

    // 第二步：DP求解TSP最短回路
    int min_cycle = tspDP(dist, n);

    // 输出结果
    cout << "\n===== 求解结果 =====" << endl;
    cout << "经过所有城市一次并返回起点的最短回路长度：" << min_cycle << endl;

    return 0;
}