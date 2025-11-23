#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <string>
using namespace std;

const int MAX_VERTEX = 20;  // 顶点数上限与 inputNum 一致（2-20）
const int MAX_EDGE = MAX_VERTEX * (MAX_VERTEX - 1) / 2;  // 无向图最大边数（完全图）

// 边的结构体：存储两个顶点和权值
struct Edge {
    int u, v, weight;
    // 排序用：按权值升序
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

// 并查集（Disjoint Set Union）：判断连通性、合并连通分量
class DSU {
private:
    vector<int> parent;  // 父节点数组
public:
    DSU(int n) {
        parent.resize(n + 1);
        for (int i = 1; i <= n; ++i) {
            parent[i] = i;  // 初始化：每个顶点的父节点是自身
        }
    }

    // 查找根节点（路径压缩优化）
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // 合并两个连通分量
    void unite(int x, int y) {
        x = find(x);
        y = find(y);
        if (x != y) {
            parent[y] = x;
        }
    }
};

// 输入并校验整数（指定范围：minVal ~ maxVal）
void inputNum(int& num, int minVal, int maxVal, const string& prompt) {
    while (true) {
        cout << prompt;
        cin >> num;
        // 校验条件：输入流正常 + 数值在指定范围
        if (cin.good() && num >= minVal && num <= maxVal) {
            break;
        }
        else {
            cin.clear();  // 清除错误状态
            cin.ignore(1000, '\n');  // 忽略缓冲区无效内容
            cout << "Input error! Please enter an integer between "
                << minVal << " and " << maxVal << ":\n";
        }
    }
}

// 输入并校验单条边（顶点合法 + 权值为正）
void inputEdge(Edge& edge, int maxVertex) {
    while (true) {
        cout << "Enter edge (u v weight): ";
        cin >> edge.u >> edge.v >> edge.weight;

        // 校验输入流状态
        if (!cin.good()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Input format error! Please enter three integers (u v weight):\n";
            continue;
        }

        // 校验顶点合法性（1 ~ maxVertex）
        if (edge.u < 1 || edge.u > maxVertex || edge.v < 1 || edge.v > maxVertex) {
            cout << "Vertex error! Vertices must be between 1 and " << maxVertex << ":\n";
            continue;
        }

        // 校验顶点是否相同（无自环边）
        if (edge.u == edge.v) {
            cout << "Error! Self-loop edges (u == v) are not allowed:\n";
            continue;
        }

        // 校验权值合法性（正整数，假设边权为正）
        if (edge.weight <= 0) {
            cout << "Weight error! Please enter a positive integer for weight:\n";
            continue;
        }

        // 所有校验通过
        break;
    }
}

// Kruskal算法核心函数：edges为边集合，n为顶点数，返回最小生成树总权值
int kruskal(vector<Edge>& edges, int n) {
    sort(edges.begin(), edges.end());  // 边按权值升序排序
    DSU dsu(n);                        // 初始化并查集
    int totalCost = 0;                 // 总权值
    int edgeCount = 0;                 // 已加入生成树的边数

    cout << "\nMinimum Spanning Tree Edges (u - v, weight):\n";

    for (const Edge& e : edges) {
        // 若两个顶点不在同一连通分量，加入该边
        if (dsu.find(e.u) != dsu.find(e.v)) {
            dsu.unite(e.u, e.v);
            totalCost += e.weight;
            edgeCount++;
            cout << e.u << " - " << e.v << " (" << e.weight << ")\n";

            // 生成树已包含n-1条边，提前结束
            if (edgeCount == n - 1) break;
        }
    }

    // 若边数不足n-1，说明图不连通（无生成树）
    if (edgeCount != n - 1) {
        cout << "\nError: The graph is disconnected, no spanning tree exists!\n";
        return -1;
    }

    return totalCost;
}

int main() {
    int n, m;  // n：顶点数，m：边数
    vector<Edge> edges;  // 存储所有边

    cout << "===== Kruskal's Algorithm for Minimum Spanning Tree =====\n";

    // 1. 输入顶点数（2-20，与 inputNum 限制一致）
    inputNum(n, 2, MAX_VERTEX, "Enter number of vertices (2-20): ");

    // 2. 输入边数（1 ~ 最大边数）
    int maxEdgeCount = MAX_EDGE;
    inputNum(m, 1, maxEdgeCount,
        "Enter number of edges (1-" + to_string(maxEdgeCount) + "): ");

    // 3. 输入m条边，逐条校验
    edges.resize(m);
    cout << "\nEnter " << m << " edges (format: u v weight), e.g., 1 2 10:\n";
    for (int i = 0; i < m; ++i) {
        cout << "Edge " << i + 1 << ": ";
        inputEdge(edges[i], n);
    }

    // 4. 调用Kruskal算法
    int minTotalCost = kruskal(edges, n);
    if (minTotalCost != -1) {
        cout << "\nTotal weight of Minimum Spanning Tree: " << minTotalCost << "\n";
    }

    return 0;
}