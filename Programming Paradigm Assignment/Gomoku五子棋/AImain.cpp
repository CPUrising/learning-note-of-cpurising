#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <cstdlib>
#include <algorithm>
using namespace std;

bool hasNeighbor(int x, int y, int distance = 2);
// 调试开关：1=启用调试输出，0=禁用调试输出
#define DEBUG_MODE 0  // 实际对战时设为0即可关闭所有调试信息

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
const int SCORE_SLEEP_FOUR = 700; // 活四(两端空)
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
    return -2000000000000000001LL;
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
void debug(const char* message) {
#if DEBUG_MODE == 1  // 仅当DEBUG_MODE为1时才输出调试信息
    printf("DEBUG %s\n", message);
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
    board[5][5] = 2;
    board[6][6] = 2;
    board[6][5] = 2;
    board[5][6] = 2;
    // 初始化当前哈希值（所有位置为空）
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

// 更新棋盘状态和哈希值
void update_board(int x, int y, int color) {
    // 移除原位置的哈希值（原状态是空）
    currentHash ^= zobristTable[EMPTY][x][y];
    // 添加新棋子的哈希值
    currentHash ^= zobristTable[color][x][y];
    // 更新棋盘状态
    board[x][y] = color;
}

// 计算指定位置在某方向的连子评分,和KAMI相比应该没问题
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

    // -------------------------- 正向检测（沿方向递增） --------------------------
    int x = posx + dx;
    int y = posy + dy;
    bool found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_p) {
            // 找到空位后，检查是否为跳连（如"1 0 1"）
            if (board[x][y] == player) {
                jump_p = 1;
                count_j_p++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_p) {
            // 跳连后的连续子检测
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
            // 未找到空位，检测连续子
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

    // -------------------------- 反向检测（沿方向递减） --------------------------
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

    // // -------------------------- 棋型判断与评分 --------------------------
    // 处理跳跃连子（如跳活三、跳活四）
    if (jump_p) {
        // 正方向跳跃
        if (count + count_j_p == 3 && empty_pos == 3 && empty_j_p) {
            return SCORE_JUMP_LIVE_THREE; // 跳活三
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && block_j_p) {
            return SCORE_JUMP_RUSH_FOUR; // 跳睡四
        }
        else if (count + count_j_p == 4 && empty_pos == 1 && empty_j_p) {
            return SCORE_JUMP_RUSH_FOUR; // 跳睡四
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && empty_j_p) {
            return SCORE_JUMP_LIVE_FOUR; // 跳活四
        }
    }
    else if (jump_n) {
        // 反方向跳跃
        if (count + count_j_n == 3 && empty_pos == 3 && empty_j_n) {
            return SCORE_JUMP_LIVE_THREE; // 跳活三
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && block_j_n) {
            return SCORE_RUSH_FOUR; // 跳睡四
        }
        else if (count + count_j_n == 4 && empty_pos == 2 && empty_j_n) {
            return SCORE_RUSH_FOUR; // 跳睡四
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && empty_j_n) {
            return SCORE_JUMP_LIVE_FOUR; // 跳活四
        }
    }

    // 处理普通连子（无跳跃）
    if (count >= 5) {
        return SCORE_FIVE;  // 五子连珠（获胜）
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR; // 活四（两端空白，必赢）
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR; // 冲四（一端空白，需补位）
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE; // 活三（两端空白，可发展为活四）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE; // 睡三（一端空白，需补位才能冲四）
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO; // 活二（两端空白，可发展为活三）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO; // 睡二（一端空白，需补位才能发展）
        }
    }

    return 0; // 无有效棋型，评分0 结果判断逻辑 -------------------------
}

// 计算指定位置的落子评分（临时落子后计算）
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    //// 临时落子，计算评分
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    LLS myScore = 0;       // AI在该位置的评分
    LLS rivalScore = 0;    // 对手在该位置的评分
    // 4个方向：水平(0,1)、垂直(1,0)、正斜(1,1)、反斜(1,-1)
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // 计算我方连子的分数
    for (auto& dir : dirs) {
        myScore += scoreDirects(x, y, dir[0], dir[1], player);
    }

    // 计算对方连子的分数（权重调整）
    for (auto& dir : dirs) {
        rivalScore += scoreDirects(x, y, dir[0], dir[1], opponent); // 防守权重
    }
    // 评分修正（同scorePlayer函数，提升中高威胁优先级）
    if (rivalScore >= 1400 && rivalScore < 1000000) {
        rivalScore = 1000000;
    }
    if (myScore >= 1400 && myScore < 1000000) {
        myScore = 1000000;
    }
    total += myScore + rivalScore;
    //// 撤销临时落子
    update_board(x, y, EMPTY);
    return total;
}


// 评估当前棋盘得分
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
                totalScore -= scorePosition(i, j, board[i][j]) ;
            }
        }
    }
    return totalScore;
}

// 带哈希优化的Alpha-Beta搜索
LLS alphaBeta(int depth, LLS alpha, LLS beta) {
    // 先检查哈希表
    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    // 搜索到最大深度，返回评估值
    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    // 生成所有可能的走法,这个方法是有问题的,需要排序并挑选6个
    vector<pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_pos(i, j) && hasNeighbor(i, j, 2)) {
                moves.emplace_back(i, j);
            }
        }
    }

    if (moves.empty()) {
        LLS score = 0; // 平局
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    LLS bestScore = -1e18;
    int currentColor = (depth % 2 == 0) ? my_color : (my_color == BLACK ? WHITE : BLACK);

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        // 落子并更新哈希
        update_board(x, y, currentColor);
        bool isWin = scorePosition(x,y, currentColor);

        LLS score;
        if (isWin) {
            // 胜利得高分
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            // 递归搜索
            score = -alphaBeta(depth - 1, -beta, -alpha);
        }

        // 撤销落子和哈希
        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
        }
        if (bestScore > alpha) {
            alpha = bestScore;
        }
        if (alpha >= beta) {
            break; // Beta剪枝
        }
    }

    // 存储哈希表
    HashType type;
    if (bestScore <= alpha) {
        type = HASH_ALPHA;
    }
    else if (bestScore >= beta) {
        type = HASH_BETA;
    }
    else {
        type = HASH_EXACT;
    }
    storeHash(currentHash, depth, bestScore, type);

    return bestScore;
}

pair<int, int> get_best_move() {
    // 优先防御对方即时胜利威胁
    /*int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY);
                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }*/

    // 生成候选落子并计算评分
    vector<pair<LLS, pair<int, int>>> candidateMoves;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            // 新增条件：位置有效 且 1-2格子内有相邻棋子
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 2)) {
                LLS score = scorePosition(x, y, my_color);
                candidateMoves.emplace_back(score, make_pair(x, y));
            }
        }
    }

    // 按评分降序排序（优先高价值位置）
    // 将原来的排序代码修改为：
    sort(candidateMoves.begin(), candidateMoves.end(),
        [](const pair<LLS, pair<int, int>>& a, const pair<LLS, pair<int, int>>& b) {
            return a.first > b.first;
        });

    // 限制候选数量（最多6个，减少探索量）
    int maxCandidates = 6;
    if (candidateMoves.size() > maxCandidates) {
        candidateMoves.resize(maxCandidates);
    }

    // 使用Alpha-Beta搜索从候选中选出最优解
    int bestX = -1, bestY = -1;
    LLS bestScore = -20000000000;
    int searchDepth = 4;

    for (const auto& candidate : candidateMoves) {
        int x = candidate.second.first;
        int y = candidate.second.second;

        update_board(x, y, my_color);
        bool isWin = scorePosition(x, y, my_color);
        LLS score;
        score = -alphaBeta(searchDepth - 1, -1e18, 1e18);        

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    // 候选搜索找到有效位置
    if (bestX != -1) {
        debug(("Optimal move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback策略
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 1)) {
                debug(("Near enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    for (int x = 4; x <= 8; x++) {
        for (int y = 4; y <= 8; y++) {
            if (is_valid_pos(x, y)) {
                return { x, y };
            }
        }
    }

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Default move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    return { -1, -1 };
}

// 处理START命令
void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock();
    debug(("Start game with color: " + to_string(my_color)).c_str());
    printf("OK\n");
    fflush(stdout);
}

// 处理PLACE命令
void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        update_board(x, y, enemy_color); // 更新棋盘和哈希
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

// 检查指定位置周围distance范围内是否有棋子
bool hasNeighbor(int x, int y, int distance ) {
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
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
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
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return;
    }

    // 执行走法
    if (is_valid_pos(x, y)) {
        update_board(x, y, my_color); // 更新棋盘和哈希
        printf("%d %d\n", x, y);
        fflush(stdout);
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
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
            debug(win_msg.c_str());
            break;
        }
        else {
            debug(("Unknown command: " + string(cmd)).c_str());
        }
    }

    delete[] hashTable; // 释放哈希表内存
    return 0;
}