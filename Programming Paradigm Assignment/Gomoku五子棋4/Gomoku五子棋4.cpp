#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <climits>   // for LLONG_MIN, LLONG_MAX
#include <map>

using namespace std;

bool hasNeighbor(int x, int y, int distance = 2);
// 调试开关：1=启用调试输出，0=禁用调试输出
#define DEBUG_MODE 1  // 实际对战时设为0即可关闭所有调试信息

// 棋盘配置
const int BOARD_SIZE = 12;    // 棋盘大小
const int BLACK = 1;          // 黑棋
const int WHITE = 2;          // 白棋
const int EMPTY = 0;          // 空位
const int MAX_SINGLE_TIME = 2; // 单步最大时间(秒)
const int MAX_TOTAL_TIME = 90; // 总时间限制(秒)
const int HASH_SIZE = 1000003; // 哈希表大小(质数)
using ULL = unsigned long long;
using LLS = long long;

// 评分配置（根据连子情况定义分数）
const int SCORE_FIVE = 1000000000;    // 五连子(胜利)
const int SCORE_LIVE_FOUR = 1100000; // 活四(两端空)
const int SCORE_JUMP_LIVE_FOUR = 1000; // 跳活四
const int SCORE_RUSH_FOUR = 1000;  // 冲四(一端空)
const int SCORE_JUMP_RUSH_FOUR = 900;  // 跳冲四
const int SCORE_LIVE_THREE = 1050;   // 活三(两端空)
const int SCORE_JUMP_LIVE_THREE = 900;  // 跳活三
const int SCORE_SLEEP_THREE = 100;   // 眠三(一端空)
const int SCORE_LIVE_TWO = 10;      // 活二
const int SCORE_SLEEP_TWO = 5;      // 眠二

// 哈希表相关定义
enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

// 哈希表结构
struct HashEntry {
    ULL key;       // 哈希值(键)
    int depth;     // 搜索深度
    LLS score;     // 搜索评分
    HashType type; // 条目类型
};

// 全局变量
vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;              // AI棋子颜色
clock_t start_total_time;      // 游戏开始时间
ULL zobristTable[3][BOARD_SIZE][BOARD_SIZE]; // Zobrist哈希表
HashEntry* hashTable;          // 主哈希表
ULL currentHash = 0;           // 当前棋盘哈希值

// 生成64位随机数
ULL random64() {
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

// 初始化Zobrist哈希表
void initZobrist() {
    srand((unsigned int)time(NULL));
    for (int piece = 0; piece < 3; piece++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    // 初始化哈希表内存
    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

// 查找哈希表
const LLS HASH_INVALID = -2000000000000000001LL;
LLS lookupHash(ULL hash, int depth, LLS alpha, LLS beta) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    if (entry.key == hash && entry.depth >= depth) {
        if (entry.type == HASH_EXACT)
            return entry.score;
        if (entry.type == HASH_ALPHA && entry.score <= alpha)
            return alpha;
        if (entry.type == HASH_BETA && entry.score >= beta)
            return beta;
    }
    return HASH_INVALID;
}

// 存储哈希表
void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    // 只存储更深层次的搜索结果
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

// 调试输出（带开关控制）
void debug(const string& message) {
#if DEBUG_MODE == 1  // 仅当DEBUG_MODE为1时才输出调试信息
    printf("DEBUG %s\n", message.c_str());
    fflush(stdout);
#endif
}

// 初始化棋盘和哈希初始值
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    // 初始开局：中心位置（和你之前设定相同）
    board[5][5] = 2;
    board[6][6] = 2;
    board[6][5] = 1;
    board[5][6] = 1;
    // 初始化当前哈希值（全部为空）
    currentHash = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            currentHash ^= zobristTable[EMPTY][i][j];
        }
    }
    debug("Board initialized successfully");
}

// 检查位置有效性
bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;
    }
    return board[x][y] == EMPTY;
}

// 更新棋盘状态和哈希值（放子/撤子均用此函数）
void update_board(int x, int y, int color) {
    // 移除原位置的哈希值（原状态是空或其它值）
    currentHash ^= zobristTable[EMPTY][x][y];
    // 添加新棋子的哈希值
    currentHash ^= zobristTable[color][x][y];
    // 更新棋盘状态
    board[x][y] = color;
}

// 计算指定位置在某方向的连子评分 (保持你原有实现)
int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;
    int count = 1;          // 当前位置的连续子数量
    int empty_pos = 0;      // 空位状态(1=一端空,3=两端空)
    int block = 0;          // 被阻挡数

    // 跳连检测变量（如"1 0 1"等间隔连子）
    int jump_p = 0, jump_n = 0;        // 正/反方向是否有跳连
    int count_j_p = 0, count_j_n = 0;  // 跳连子数量
    bool block_j_p = false, block_j_n = false; // 跳连是否被挡
    bool empty_j_p = false, empty_j_n = false; // 跳连是否有空位

    // 正向检测
    int x = posx + dx;
    int y = posy + dy;
    bool found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_p) {
            if (board[x][y] == player) {
                jump_p = 1;
                count_j_p++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_p) {
            if (board[x][y] == player) {
                count_j_p++;
            }
            else if (board[x][y] == opponent) {
                block_j_p = true;
                break;
            }
            else {
                empty_j_p = true;
                break;
            }
        }
        else {
            if (board[x][y] == EMPTY) {
                empty_pos += 1;  // 一端空
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // 连续子+1
            }
        }
        x += dx;
        y += dy;
    }

    // 反向检测
    x = posx - dx;
    y = posy - dy;
    found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_n) {
            if (board[x][y] == player) {
                jump_n = 1;
                count_j_n++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_n) {
            if (board[x][y] == player) {
                count_j_n++;
            }
            else if (board[x][y] == opponent) {
                block_j_n = true;
                break;
            }
            else {
                empty_j_n = true;
                break;
            }
        }
        else {
            if (board[x][y] == EMPTY) {
                empty_pos += 2;  // 两端空（总和为3）
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // 连续子+1
            }
        }
        x -= dx;
        y -= dy;
    }

    // 跳连判断
    if (jump_p) {
        if (count + count_j_p == 3 && empty_pos == 3 && empty_j_p) return SCORE_JUMP_LIVE_THREE;
        else if (count + count_j_p == 4 && empty_pos == 3 && block_j_p) return SCORE_JUMP_RUSH_FOUR;
        else if (count + count_j_p == 4 && empty_pos == 1 && empty_j_p) return SCORE_JUMP_RUSH_FOUR;
        else if (count + count_j_p == 4 && empty_pos == 3 && empty_j_p) return SCORE_JUMP_LIVE_FOUR;
    }
    else if (jump_n) {
        if (count + count_j_n == 3 && empty_pos == 3 && empty_j_n) return SCORE_JUMP_LIVE_THREE;
        else if (count + count_j_n == 4 && empty_pos == 3 && block_j_n) return SCORE_RUSH_FOUR;
        else if (count + count_j_n == 4 && empty_pos == 2 && empty_j_n) return SCORE_RUSH_FOUR;
        else if (count + count_j_n == 4 && empty_pos == 3 && empty_j_n) return SCORE_JUMP_LIVE_FOUR;
    }

    // 普通连子判断
    if (count >= 5) return SCORE_FIVE;
    else if (count == 4) {
        if (empty_pos == 3) return SCORE_LIVE_FOUR;
        else if (empty_pos > 0) return SCORE_RUSH_FOUR;
    }
    else if (count == 3) {
        if (empty_pos == 3) return SCORE_LIVE_THREE;
        else if (empty_pos > 0) return SCORE_SLEEP_THREE;
    }
    else if (count == 2) {
        if (empty_pos == 3) return SCORE_LIVE_TWO;
        else if (empty_pos > 0) return SCORE_SLEEP_TWO;
    }

    return 0;
}

// --------------- 新增：KAMI式棋型表与识别 ---------------

struct ChessStyle {
    LLS score;
    ChessStyle(LLS s = 0) : score(s) {}
};

struct AllChessStyle {
    ChessStyle MyStyle[11];
    ChessStyle EnemyStyle[11];
    AllChessStyle() {
        MyStyle[0].score = 0;         // 无
        MyStyle[1].score = 10;        // 睡二
        MyStyle[2].score = 100;       // 活二
        MyStyle[3].score = 100;       // 睡三
        MyStyle[4].score = 1050;      // 活三
        MyStyle[5].score = 1000;      // 冲四
        MyStyle[6].score = 1100000;   // 活四
        MyStyle[7].score = 1000000000; // 五（非常大）
        MyStyle[8].score = 900;       // 跳活三
        MyStyle[9].score = 700;       // 跳睡四
        MyStyle[10].score = 1000;     // 跳活四
        for (int i = 0; i < 11; ++i) {
            // 敌方权重略高，促使优先防守
            EnemyStyle[i].score = (LLS)(MyStyle[i].score * 1.2);
        }
    }
};

// 双向棋型识别（返回0..10）
int detectStyle(int x, int y, int dx, int dy, int color) {
    int count = 1;
    int openEnds = 0;

    // 正方向
    int nx = x + dx, ny = y + dy;
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
        count++;
        nx += dx; ny += dy;
    }
    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == EMPTY) openEnds++;

    // 反方向
    nx = x - dx; ny = y - dy;
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
        count++;
        nx -= dx; ny -= dy;
    }
    if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == EMPTY) openEnds++;

    if (count >= 5) return 7;                 // 成五
    if (count == 4 && openEnds == 2) return 6; // 活四
    if (count == 4 && openEnds == 1) return 5; // 冲四
    if (count == 3 && openEnds == 2) return 4; // 活三
    if (count == 3 && openEnds == 1) return 3; // 睡三
    if (count == 2 && openEnds == 2) return 2; // 活二
    if (count == 2 && openEnds == 1) return 1; // 睡二
    return 0;
}

// 计算指定位置的落子评分（使用棋型表 + 防守惩罚）
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    // 临时落子
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS myScore = 0;       // AI在该位置的评分
    LLS rivalScore = 0;    // 对手在该位置的评分
    AllChessStyle styles;
    int dirs[4][2] = { {0,1}, {1,0}, {1,1}, {1,-1} };

    // 逐方向检测棋型并查表评分
    for (int i = 0; i < 4; ++i) {
        int dx = dirs[i][0], dy = dirs[i][1];
        int myStyle = detectStyle(x, y, dx, dy, player);
        int rivalStyle = detectStyle(x, y, dx, dy, opponent);
        myScore += styles.MyStyle[myStyle].score;
        rivalScore += styles.EnemyStyle[rivalStyle].score;
    }

    // 撤销临时落子
    update_board(x, y, EMPTY);

    // 阈值放大（把中等威胁提升为高优先级）
    if (rivalScore >= 1400 && rivalScore < 1000000) {
        rivalScore = 1000000;
    }
    if (myScore >= 1400 && myScore < 1000000) {
        myScore = 1000000;
    }

    // 综合评分：以防守优先 (进攻 - 对手威胁)
    LLS total = myScore - rivalScore;

    // 对敌方高威胁额外惩罚（强制优先防守）
    if (rivalScore >= styles.EnemyStyle[6].score) total -= 12000000; // 敌方活四
    else if (rivalScore >= styles.EnemyStyle[5].score) total -= 10000000; // 敌方冲四
    else if (rivalScore >= styles.EnemyStyle[4].score) total -= 6000000; // 敌方活三
    // 如果敌方落此点能立刻赢，则立即强制防守
    for (int d = 0; d < 4; ++d) {
        int sEnemy = detectStyle(x, y, dirs[d][0], dirs[d][1], opponent);
        if (sEnemy >= 7) {
            total -= 50000000; // 必防立即胜点
            break;
        }
        else if (sEnemy == 6) {
            total -= 30000000; // 活四威胁
        }
        else if (sEnemy == 5) {
            total -= 15000000; // 冲四威胁
        }
    }
    // 边缘惩罚（靠近边界的点价值下降）
    if (x < 2 || y < 2 || x > BOARD_SIZE - 3 || y > BOARD_SIZE - 3)
        total -= 5000;

    return total;
}

// 评估当前棋盘得分（保留原 evaluate 逻辑调用新版 scorePosition）
LLS evaluate() {
    LLS totalScore = 0;
    // 遍历所有落子位置计算分数
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                // 累加我方棋子的得分
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                // 减去对方棋子的得分（防守权重）
                totalScore -= scorePosition(i, j, board[i][j]);
            }
        }
    }
    return totalScore;
}

// ------------------ alphaBeta（negamax 风格）实现 ------------------
// 使用全局 currentHash / hashTable / lookupHash / storeHash / evaluate / scorePosition
LLS alphaBeta(int depth, LLS alpha, LLS beta) {
    // 1) 哈希查找
    LLS hashVal = lookupHash(currentHash, depth, alpha, beta);
    if (hashVal != HASH_INVALID) {
        return hashVal;
    }

    // 2) 到达叶节点：评估
    if (depth == 0) {
        LLS v = evaluate();
        storeHash(currentHash, depth, v, HASH_EXACT);
        return v;
    }

    // 当前移动方：根据 depth 奇偶决定（与原代码风格保持一致）
    int opponent = (my_color == BLACK) ? WHITE : BLACK;
    int currentColor = (depth % 2 == 0) ? my_color : opponent;

    // 3) 生成候选走法（邻域1格，按scorePosition排序）
    vector<pair<LLS, pair<int, int>>> candidateMoves;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (is_valid_pos(i, j) && hasNeighbor(i, j, 1)) {
                LLS sc = scorePosition(i, j, currentColor);
                candidateMoves.emplace_back(sc, make_pair(i, j));
            }
        }
    }

    // 若无候选，返回评估分
    if (candidateMoves.empty()) {
        LLS v = evaluate();
        storeHash(currentHash, depth, v, HASH_EXACT);
        return v;
    }

    // 排序并截取前6个
    sort(candidateMoves.begin(), candidateMoves.end(),
        [](const pair<LLS, pair<int, int>>& a, const pair<LLS, pair<int, int>>& b) {
            return a.first > b.first;
        });
    if (candidateMoves.size() > 6) candidateMoves.resize(6);

    LLS bestValue = -9e18;
    LLS alphaOrig = alpha;
    HashType storeType = HASH_ALPHA;

    // 4) 遍历候选进行递归（negamax）
    for (auto& cm : candidateMoves) {
        int x = cm.second.first;
        int y = cm.second.second;

        // 落子
        update_board(x, y, currentColor);

        // 是否直接获胜
        bool isWin = (scorePosition(x, y, currentColor) >= SCORE_FIVE);
        LLS val;
        if (isWin) {
            // 如果当前色是我方则很大正值，否则很小负值
            val = (currentColor == my_color) ? (LLS)1e18 : (LLS)-1e18;
        }
        else {
            // 递归：切换视角，取负值
            val = -alphaBeta(depth - 1, -beta, -alpha);
        }

        // 撤子
        update_board(x, y, EMPTY);

        if (val > bestValue) bestValue = val;
        if (bestValue > alpha) {
            alpha = bestValue;
            storeType = HASH_EXACT;
        }
        if (alpha >= beta) {
            storeType = HASH_BETA;
            break; // Beta 剪枝
        }
    }

    // 存哈希
    if (bestValue <= alphaOrig) storeType = HASH_ALPHA;
    else if (bestValue >= beta) storeType = HASH_BETA;
    else storeType = HASH_EXACT;

    storeHash(currentHash, depth, bestValue, storeType);
    return bestValue;
}

pair<int,int> get_best_move() {
    int enemy = (my_color == BLACK) ? WHITE : BLACK;
    int dirs[4][2] = {{0,1},{1,0},{1,1},{1,-1}};

    // 0) 我方即胜扫描 —— 若我方某步能直接赢，立刻返回
    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (!is_valid_pos(x,y)) continue;
            update_board(x, y, my_color);
            bool win = false;
            for (int d = 0; d < 4; ++d) {
                int s = detectStyle(x, y, dirs[d][0], dirs[d][1], my_color);
                if (s >= 7) { win = true; break; }   // 成五
                if (s == 6) { win = true; break; }   // 活四 -> 也可视为马上赢
            }
            update_board(x, y, EMPTY);
            if (win) {
#if DEBUG_MODE==1
                printf("DEBUG: immediate WIN at (%d,%d)\n", x, y); fflush(stdout);
#endif
                return {x,y};
            }
        }
    }

    // 1) 敌方即胜/高威胁扫描（包含：敌方落某点能成五/活四/冲四；双活三；并且检查盘上已有的未封口四连）
    struct Threat { int x,y; int priority; }; // priority 越高越急
    vector<Threat> threats;

    // (A) 检测“若敌方在空位落子会立刻赢/高威胁/双活三”
    for (int x = 0; x < BOARD_SIZE; ++x) {
        for (int y = 0; y < BOARD_SIZE; ++y) {
            if (!is_valid_pos(x,y)) continue;
            update_board(x,y,enemy);
            int highThreatCount = 0;
            int liveThreeCount = 0;
            int maxStyle = 0;
            for (int d=0; d<4; ++d) {
                int s = detectStyle(x,y,dirs[d][0],dirs[d][1],enemy);
                if (s >= 4) highThreatCount++;   // >=4 (包括活三/冲四/活四)
                if (s == 4) liveThreeCount++;
                if (s > maxStyle) maxStyle = s;
            }
            update_board(x,y,EMPTY);

            int priority = 0;
            if (maxStyle >= 7) priority = 10000000;       // 成五
            else if (highThreatCount >= 2) priority = 9000000; // 双方向高威胁
            else if (highThreatCount >= 1 && liveThreeCount >=1) priority = 8000000; // 组合威胁
            else if (highThreatCount >= 1) priority = 7000000; // 冲四/活三/活四
            else if (liveThreeCount >=2) priority = 6000000; // 双活三
            if (priority > 0) threats.push_back({x,y,priority});
        }
    }

    // (B) 扫描盘面已有连续的 enemy 连子（直接找已有的连 4），并加入两端空点作为高优先
    for (int i=0; i<BOARD_SIZE; ++i) {
        for (int j=0; j<BOARD_SIZE; ++j) {
            if (board[i][j] != enemy) continue;
            for (int d=0; d<4; ++d) {
                int dx = dirs[d][0], dy = dirs[d][1];
                // only consider a sequence head to avoid duplicates
                if (is_valid_pos(i-dx, j-dy) && board[i-dx][j-dy] == enemy) continue;
                // count sequence length
                int cnt = 0, tx = i, ty = j;
                while (is_valid_pos(tx,ty) && board[tx][ty]==enemy) { cnt++; tx+=dx; ty+=dy; }
                if (cnt >= 4) {
                    // two ends
                    int ex1x = i - dx, ex1y = j - dy;
                    int ex2x = tx, ex2y = ty;
                    if (is_valid_pos(ex1x,ex1y) && board[ex1x][ex1y]==EMPTY) threats.push_back({ex1x,ex1y,9500000});
                    if (is_valid_pos(ex2x,ex2y) && board[ex2x][ex2y]==EMPTY) threats.push_back({ex2x,ex2y,9500000});
                }
            }
        }
    }

    // 如果有威胁点，按优先级选最能阻断的点（尽量选能阻多个方向的点）
    if (!threats.empty()) {
        // 合并相同坐标的优先级（取最大）
        map<pair<int,int>, int> bestPr;
        for (auto &t : threats) {
            auto key = make_pair(t.x,t.y);
            if (!bestPr.count(key) || t.priority > bestPr[key]) bestPr[key] = t.priority;
        }
        vector<pair<pair<int,int>,int>> merged;
        for (auto &p : bestPr) merged.emplace_back(p.first, p.second);
        sort(merged.begin(), merged.end(), [](const pair<pair<int,int>,int>& a, const pair<pair<int,int>,int>& b){
            if (a.second != b.second) return a.second > b.second;
            // tiebreak: choose nearer board center to prefer central block
            int ax = a.first.first, ay = a.first.second, bx = b.first.first, by = b.first.second;
            int ca = abs(ax-BOARD_SIZE/2)+abs(ay-BOARD_SIZE/2);
            int cb = abs(bx-BOARD_SIZE/2)+abs(by-BOARD_SIZE/2);
            return ca < cb;
        });

        // 选择第一个可行的防守点并返回（最紧急）
        auto chosen = merged.front().first;
#if DEBUG_MODE==1
        printf("DEBUG: must block at (%d,%d) with priority %d\n", chosen.first, chosen.second, merged.front().second); fflush(stdout);
#endif
        return {chosen.first, chosen.second};
    }

    // 2) 没有立即威胁，生成候选集合（邻域1或2），并把能延伸己方活三/阻断敌方高威胁的点加大权重
    vector<pair<LLS,pair<int,int>>> candidates;
    for (int x=0; x<BOARD_SIZE; ++x) {
        for (int y=0; y<BOARD_SIZE; ++y) {
            if (!is_valid_pos(x,y) || !hasNeighbor(x,y,2)) continue;
            LLS base = scorePosition(x,y,my_color);
            LLS bonus = 0;

            // 若我方下此点可生成/延伸活三或冲四 -> 鼓励
            for (int d=0; d<4; ++d) {
                int sMine = detectStyle(x,y,dirs[d][0],dirs[d][1], my_color);
                if (sMine >= 4) bonus += 2500000LL;
                if (sMine == 6) bonus += 5000000LL; // 活四更鼓励
            }
            // 若此点能阻断敌方在此点落子会造成高威胁，给极大bonus（确保进入候选）
            for (int d=0; d<4; ++d) {
                int sEnemy = detectStyle(x,y,dirs[d][0],dirs[d][1], enemy);
                if (sEnemy >= 5) bonus += 15000000LL;
                if (sEnemy == 4) bonus += 5000000LL; // 敌方活三也要防
            }
            // 轻微边界惩罚，偏好中心
            int borderPenalty = 0;
            if (x < 2 || y < 2 || x >= BOARD_SIZE-2 || y >= BOARD_SIZE-2) borderPenalty = 2000;

            candidates.emplace_back(base + bonus - borderPenalty, make_pair(x,y));
        }
    }

    if (candidates.empty()) {
        // fallback
        for (int x=0;x<BOARD_SIZE;++x) for (int y=0;y<BOARD_SIZE;++y) if (is_valid_pos(x,y)) return {x,y};
        return {-1,-1};
    }

    sort(candidates.begin(), candidates.end(), [](const pair<LLS,pair<int,int>>& a, const pair<LLS,pair<int,int>>& b){
        return a.first > b.first;
    });

    // keep more candidates than before to avoid pruning掉防守点
    int maxCandidates = min((int)candidates.size(), 12);
    candidates.resize(maxCandidates);

    // 3) 对候选进行 alphaBeta 搜索（negamax 风格）
    int bestX=-1, bestY=-1;
    LLS bestScore = LLONG_MIN/4;
    int searchDepth = 5; // 你可调整为 4/5/6 视耗时而定

    for (auto &cm : candidates) {
        int x = cm.second.first, y = cm.second.second;
        update_board(x,y,my_color);
        bool win = (scorePosition(x,y,my_color) >= SCORE_FIVE);
        LLS val;
        if (win) val = (LLS)1e17;
        else val = -alphaBeta(searchDepth-1, -1e18, 1e18);
        update_board(x,y,EMPTY);

        if (val > bestScore) {
            bestScore = val;
            bestX = x; bestY = y;
        }
    }

#if DEBUG_MODE==1
    printf("DEBUG: chosen (%d,%d) by search score=%lld\n", bestX, bestY, bestScore); fflush(stdout);
#endif

    if (bestX != -1) return {bestX,bestY};
    // fallback
    for (int x=0;x<BOARD_SIZE;++x) for (int y=0;y<BOARD_SIZE;++y) if (is_valid_pos(x,y)) return {x,y};
    return {-1,-1};
}



// 处理START命令
void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock();
    debug("Start game");
    printf("OK\n");
    fflush(stdout);
}

// 处理PLACE命令
void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        update_board(x, y, enemy_color); // 更新棋盘和哈希
        debug("Enemy placed");
    }
    else {
        debug("Invalid enemy move");
    }
}

// 检查指定位置周围distance范围内是否有棋子
bool hasNeighbor(int x, int y, int distance) {
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            if (dx == 0 && dy == 0) continue; // 跳过自身

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board[nx][ny] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}

// 打印有效候选走法（调试用）
void printValidMoves() {
#if DEBUG_MODE == 1  // 仅调试模式下打印
    vector<pair<int, int>> moves;
    for (int x = 0; x < BOARD_SIZE; ++x)
        for (int y = 0; y < BOARD_SIZE; ++y)
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 1))
                moves.emplace_back(x, y);
    printf("Valid moves count: %d\n", (int)moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
#endif
}

// 处理TURN命令
void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves(); // 仅调试模式下打印

    // 获取最佳走法
    pair<int, int> move = get_best_move();
    int x = move.first, y = move.second;

    // 检查单步时间
    int single_used = (clock() - start_single_time) / CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug("Single move time exceeded");
        return;
    }

    // 执行走法
    if (is_valid_pos(x, y)) {
        update_board(x, y, my_color); // 更新棋盘和哈希
        printf("%d %d\n", x, y);
        fflush(stdout);
        debug("My move executed");
    }
    else {
        debug("Invalid move generated!");
        // 寻找第一个空位
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (is_valid_pos(i, j)) {
                    update_board(i, j, my_color);
                    printf("%d %d\n", i, j);
                    fflush(stdout);
                    return;
                }
            }
        }
    }
}

int main() {
    initZobrist(); // 初始化Zobrist哈希表
    debug("Brain program started");
    char cmd[20];

    while (true) {
        if (scanf("%s", cmd) != 1) {
            continue;
        }

        if (strcmp(cmd, "START") == 0) {
            int color;
            scanf("%d", &color);
            handle_start(color);
        }
        else if (strcmp(cmd, "PLACE") == 0) {
            int x, y;
            scanf("%d %d", &x, &y);
            handle_place(x, y);
        }
        else if (strcmp(cmd, "TURN") == 0) {
            handle_turn();
        }
        else if (strcmp(cmd, "END") == 0) {
            int winner;
            scanf("%d", &winner);
            string win_msg = (winner == 0) ? "Draw game" :
                (winner == my_color) ? "I won" : "Enemy won";
            debug(win_msg);
            break;
        }
        else {
            debug(string("Unknown command: ") + string(cmd));
        }
    }

    delete[] hashTable; // 释放哈希表内存
    return 0;
}
