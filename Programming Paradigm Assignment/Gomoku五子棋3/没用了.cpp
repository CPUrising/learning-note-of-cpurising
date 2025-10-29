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
const int SCORE_FIVE = 1000000;    // 五连子(胜利)
const int SCORE_LIVE_FOUR = 100000; // 活四(两端空)
const int SCORE_JUMP_LIVE_FOUR = 90000; // 跳活四
const int SCORE_RUSH_FOUR = 10000;  // 冲四(一端空)
const int SCORE_JUMP_RUSH_FOUR = 9000;  // 跳冲四
const int SCORE_LIVE_THREE = 10000;   // 活三(两端空)
const int SCORE_JUMP_LIVE_THREE = 5000;  // 跳活三
const int SCORE_SLEEP_THREE = 100;   // 眠三(一端空)
const int SCORE_LIVE_TWO = 10;      // 活二
const int SCORE_SLEEP_TWO = 5;      // 眠二

// 定义落子结构体
struct Move {
    int x, y;
    Move(int x_, int y_) : x(x_), y(y_) {}
};

// 候选落子列表
struct MoveList {
    vector<Move> moves;
    int count() const { return moves.size(); }
};
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
    board[6][5] = 1;
    board[5][6] = 1;
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

    // -------------------------- 结果判断逻辑 --------------------------
    // 处理跳连情况
    if (jump_p || jump_n) {
        int total_jump = count + max(count_j_p, count_j_n);
        if (total_jump == 4) {
            if ((jump_p && empty_j_p && empty_pos == 3) || (jump_n && empty_j_n && empty_pos == 3)) {
                return SCORE_JUMP_LIVE_FOUR; // 跳活四
            }
            else if ((jump_p && (block_j_p || empty_j_p)) || (jump_n && (block_j_n || empty_j_n))) {
                return SCORE_JUMP_RUSH_FOUR; // 跳冲四
            }
        }
        else if (total_jump == 3 && empty_pos == 3) {
            return SCORE_JUMP_LIVE_THREE; // 跳活三
        }
    }

    // 处理普通连子（非跳连）
    if (count >= 5) {
        return SCORE_FIVE; // 五连子
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR; // 活四（两端空）
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR; // 冲四（一端空）
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE; // 活三（两端空）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE; // 眠三（一端空）
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO; // 活二（两端空）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO; // 眠二（一端空）
        }
    }

    return 0; // 无有效连子
}

// 计算指定位置的落子评分（临时落子后计算）
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    // 临时落子，计算评分
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    // 4个方向：水平(0,1)、垂直(1,0)、正斜(1,1)、反斜(1,-1)
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // 计算我方连子的分数
    for (auto& dir : dirs) {
        total += scoreDirects(x, y, dir[0], dir[1], player);
    }

    // 计算对方连子的分数（权重调整）
    for (auto& dir : dirs) {
        total -= scoreDirects(x, y, dir[0], dir[1], opponent) * 0.9; // 防守权重
    }

    // 撤销临时落子
    update_board(x, y, EMPTY);
    return total;
}

// 检查是否胜利
bool check_win(int x, int y, int color) {
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1;

        // 正向计数
        for (int i = 1; i < 5; i++) {
            int nx = x + dx * i;
            int ny = y + dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
                count++;
            }
            else {
                break;
            }
        }

        // 反向计数
        for (int i = 1; i < 5; i++) {
            int nx = x - dx * i;
            int ny = y - dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
                count++;
            }
            else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }
    return false;
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
                totalScore -= scorePosition(i, j, board[i][j]) * 0.9;
            }
        }
    }
    return totalScore;
}

// 带哈希优化的Alpha-Beta搜索

// 生成候选落子列表（仅考虑有效位置且有邻居的点）
void getMoves(MoveList& moves, int player) {
    moves.moves.clear();
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 2)) {
                moves.moves.emplace_back(x, y);
            }
        }
    }

    // 若没有候选落子，添加中心区域备选
    if (moves.count() == 0) {
        for (int x = 4; x <= 7; x++) {
            for (int y = 4; y <= 7; y++) {
                if (is_valid_pos(x, y)) {
                    moves.moves.emplace_back(x, y);
                }
            }
        }
    }

    // 按评分排序（提升剪枝效率）
    sort(moves.moves.begin(), moves.moves.end(), [&](const Move& a, const Move& b) {
        return scorePosition(a.x, a.y, player) > scorePosition(b.x, b.y, player);
        });
}
// MinMax算法核心（带Alpha-Beta剪枝）
LLS minMax(int depth, int player, LLS alpha, LLS beta) {
    // 哈希查找
    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    // 到达最大深度，返回当前评分
    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    // 生成候选落子
    MoveList moves;
    getMoves(moves, player);
    HashType hashType = HASH_ALPHA;

    // 无候选落子，返回当前评分
    if (moves.count() == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    int opponent = (player == BLACK) ? WHITE : BLACK;

    // 最大化玩家（AI）
    if (player == my_color) {
        LLS maxEval = -1e18;
        for (const auto& move : moves.moves) {
            ULL oldHash = currentHash;

            // 模拟落子
            update_board(move.x, move.y, player);
            bool isWin = check_win(move.x, move.y, player);
            LLS eval;

            if (isWin) {
                eval = 1e18; // 获胜评分
            }
            else {
                eval = minMax(depth - 1, opponent, alpha, beta);
            }

            // 回溯
            update_board(move.x, move.y, EMPTY);
            currentHash = oldHash;

            // 更新最大值
            if (eval > maxEval) {
                maxEval = eval;
                if (eval > alpha) {
                    alpha = eval;
                    hashType = HASH_EXACT;
                }
            }

            // Beta剪枝
            if (beta <= alpha) {
                hashType = HASH_BETA;
                break;
            }
        }

        storeHash(currentHash, depth, maxEval, hashType);
        return maxEval;
    }
    // 最小化玩家（对手）
    else {
        LLS minEval = 1e18;
        for (const auto& move : moves.moves) {
            ULL oldHash = currentHash;

            // 模拟落子
            update_board(move.x, move.y, player);
            bool isWin = check_win(move.x, move.y, player);
            LLS eval;

            if (isWin) {
                eval = -1e18; // 对手获胜评分
            }
            else {
                eval = minMax(depth - 1, opponent, alpha, beta);
            }

            // 回溯
            update_board(move.x, move.y, EMPTY);
            currentHash = oldHash;

            // 更新最小值
            if (eval < minEval) {
                minEval = eval;
                if (eval < beta) {
                    beta = eval;
                    hashType = HASH_EXACT;
                }
            }

            // Alpha剪枝
            if (beta <= alpha) {
                hashType = HASH_ALPHA;
                break;
            }
        }

        storeHash(currentHash, depth, minEval, hashType);
        return minEval;
    }
}
pair<int, int> get_best_move() {
    // 优先防守对手获胜点
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY);
                if (enemy_win) {
                    return { x, y };
                }
            }
        }
    }

    // 生成候选落子
    MoveList moves;
    getMoves(moves, my_color);
    if (moves.count() == 0) {
        return { 5, 5 }; // 默认中心位置
    }

    // 计算最优落子
    LLS bestScore = -1e18;
    pair<int, int> bestMove(-1, -1);
    int searchDepth = 4;

    for (const auto& move : moves.moves) {
        ULL oldHash = currentHash;

        // 模拟落子
        update_board(move.x, move.y, my_color);
        bool isWin = check_win(move.x, move.y, my_color);
        LLS score;

        if (isWin) {
            score = 1e18;
        }
        else {
            score = minMax(searchDepth - 1, enemy_color, -1e18, 1e18);
        }

        // 回溯
        update_board(move.x, move.y, EMPTY);
        currentHash = oldHash;

        // 更新最优解
        if (score > bestScore) {
            bestScore = score;
            bestMove = { move.x, move.y };
        }
    }

    return bestMove;
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



//// 打印有效候选走法（调试用）
//void printValidMoves() {
//#if DEBUG_MODE == 1  // 仅调试模式下打印
//    vector<pair<int, int>> moves = getMoves();
//    printf("Valid moves count: %d\n", moves.size());
//    for (auto& move : moves) {
//        printf("(%d, %d) ", move.first, move.second);
//    }
//    printf("\n");
//    fflush(stdout);
//#endif
//}

// 处理TURN命令
void handle_turn() {
    clock_t start_single_time = clock();
    //printValidMoves(); // 仅调试模式下打印

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