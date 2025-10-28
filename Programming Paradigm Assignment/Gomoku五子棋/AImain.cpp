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

// 常量定义
const int BOARD_SIZE = 12;    // 棋盘尺寸
const int BLACK = 1;          // 黑棋
const int WHITE = 2;          // 白棋
const int EMPTY = 0;          // 空位
const int MAX_SINGLE_TIME = 2; // 单步最大时间(秒)
const int MAX_TOTAL_TIME = 90; // 总最大时间(秒)
const int HASH_SIZE = 1000003; // 哈希表大小(质数)
using ULL = unsigned long long;
using LLS = long long;

// 棋型评分常量（数值可根据策略调整）
const int SCORE_FIVE = 1000000;    // 五子连珠（必胜）
const int SCORE_LIVE_FOUR = 100000; // 活四（两端空白，必赢）
const int SCORE_JUMP_LIVE_FOUR = 90000; // 跳活四
const int SCORE_RUSH_FOUR = 10000;  // 冲四（一端空白）
const int SCORE_JUMP_RUSH_FOUR = 9000;  // 跳冲四（跳睡四）
const int SCORE_LIVE_THREE = 1000;   // 活三（两端空白）
const int SCORE_JUMP_LIVE_THREE = 900;  // 跳活三
const int SCORE_SLEEP_THREE = 100;   // 睡三（一端空白）
const int SCORE_LIVE_TWO = 10;      // 活二
const int SCORE_SLEEP_TWO = 5;      // 睡二

// 哈希表条目类型
enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

// 哈希表条目结构
struct HashEntry {
    ULL key;       // 哈希值(键)
    int depth;     // 搜索深度
    LLS score;     // 评估分数
    HashType type; // 条目类型
};

// 全局变量
vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;              // AI棋子颜色
clock_t start_total_time;      // 游戏开始时间
ULL zobristTable[3][BOARD_SIZE][BOARD_SIZE]; // Zobrist哈希表
HashEntry* hashTable;          // 缓存哈希表
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

    // 初始化哈希缓存表
    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

// 查找哈希缓存
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

// 存储哈希缓存
void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    // 只存储更深层的搜索结果
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

// 调试输出
void debug(const char* message) {
    printf("DEBUG %s\n", message);
    fflush(stdout);
}

// 初始化棋盘（包含哈希初始化）
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
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

// 更新棋盘并同步哈希值
void update_board(int x, int y, int color) {
    // 先移除原位置的哈希值（原先是空）
    currentHash ^= zobristTable[EMPTY][x][y];
    // 添加新棋子的哈希值
    currentHash ^= zobristTable[color][x][y];
    // 更新棋盘状态
    board[x][y] = color;
}

// 辅助函数：检查位置(posx, posy)在方向(dx, dy)上的棋型评分
int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;
    int count = 1;          // 当前位置的棋子计入连子数
    int empty_pos = 0;      // 空白状态（1=单端空，3=两端空）
    int block = 0;          // 被阻挡次数

    // 跳跃连子相关变量（处理"101"等间隔连子）
    int jump_p = 0, jump_n = 0;        // 正/反方向是否有跳跃
    int count_j_p = 0, count_j_n = 0;  // 跳跃连子数
    bool block_j_p = false, block_j_n = false; // 跳跃后是否被挡
    bool empty_j_p = false, empty_j_n = false; // 跳跃后是否空白

    // -------------------------- 正方向检查（如向右、向下） --------------------------
    int x = posx + dx;
    int y = posy + dy;
    bool found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_p) {
            // 已找到空白，检查是否为跳跃连子（如"1 0 1"）
            if (board[x][y] == player) {
                jump_p = 1;
                count_j_p++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_p) {
            // 跳跃后继续检查
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
            // 未找到空白，检查连续连子
            if (board[x][y] == EMPTY) {
                empty_pos += 1;  // 单端空白
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // 己方连子+1
            }
        }
        x += dx;
        y += dy;
    }

    // -------------------------- 反方向检查（如向左、向上） --------------------------
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
                empty_pos += 2;  // 两端空白（累计为3）
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // 己方连子+1
            }
        }
        x -= dx;
        y -= dy;
    }

    // -------------------------- 棋型判断与评分 --------------------------
    // 处理跳跃连子
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

    // 处理普通连子（无跳跃）
    if (count >= 5) {
        return SCORE_FIVE; // 五子连珠
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
            return SCORE_SLEEP_THREE; // 睡三（一端空）
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO; // 活二（两端空）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO; // 睡二（一端空）
        }
    }

    return 0; // 无有效棋型
}

// 计算指定位置落子后的综合评分（区分敌我）
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    // 临时落子，用于评分计算
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    // 4个方向：横(0,1)、竖(1,0)、斜下(1,1)、斜上(1,-1)
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // 计算己方棋型得分
    for (auto& dir : dirs) {
        total += scoreDirects(x, y, dir[0], dir[1], player);
    }

    // 计算敌方棋型得分（权重可调整，通常需要优先防守）
    for (auto& dir : dirs) {
        total -= scoreDirects(x, y, dir[0], dir[1], opponent) * 0.9; // 防守权重略低
    }

    // 撤销临时落子
    update_board(x, y, EMPTY);
    return total;
}
// 检查是否获胜
bool check_win(int x, int y, int color) {
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1;

        // 正向检查
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

        // 反向检查
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

// 评估函数（示例）
// 新的评估函数：基于棋型评分
LLS evaluate() {
    LLS totalScore = 0;
    // 对棋盘所有非空位置进行评分
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                // 累加己方已有棋子的棋型分
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                // 减去敌方已有棋子的棋型分（防守）
                totalScore -= scorePosition(i, j, board[i][j]) * 0.9;
            }
        }
    }
    return totalScore;
}

// 带哈希缓存的搜索（简化版Alpha-Beta）
LLS alphaBeta(int depth, LLS alpha, LLS beta) {
    // 先检查哈希缓存
    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    // 到达最大深度，返回评估值
    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    // 生成所有可能的落子
    vector<pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_pos(i, j)) {
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
        bool isWin = check_win(x, y, currentColor);

        LLS score;
        if (isWin) {
            // 获胜得高分
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            // 递归搜索
            score = -alphaBeta(depth - 1, -beta, -alpha);
        }

        // 撤销落子并恢复哈希
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

    // 存储哈希结果
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

// 获取最佳落子（使用哈希缓存优化）
pair<int, int> get_best_move() {
    // 检查是否有必防位置（优先逻辑）
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY); // 恢复

                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 使用带哈希缓存的搜索算法
    int bestX = -1, bestY = -1;
    LLS bestScore = -1e18;
    int searchDepth = 3; // 搜索深度

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                // 尝试落子
                update_board(x, y, my_color);
                bool isWin = check_win(x, y, my_color);
                LLS score;

                if (isWin) {
                    score = 1e18; // 获胜走法
                }
                else {
                    // 搜索对手的最佳应对
                    score = -alphaBeta(searchDepth - 1, -1e18, 1e18);
                }

                // 恢复棋盘
                update_board(x, y, EMPTY);

                if (score > bestScore) {
                    bestScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }
    }

    // 如果搜索到有效位置
    if (bestX != -1) {
        debug(("Hash-optimized move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback逻辑：中心位置
    for (int x = 5; x <= 7; x++) {
        for (int y = 5; y <= 7; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Occupy center at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    // 最后的默认走法
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
        update_board(x, y, enemy_color); // 使用带哈希更新的函数
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

// 检查指定位置周围1-2格内是否有棋子
bool hasNeighbor(int x, int y, int distance = 2) {
    // 检查周围distance范围内的所有位置
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            // 跳过当前位置自身
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            // 检查是否在棋盘范围内且有棋子
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board[nx][ny] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}

// 获取所有有效的候选落子位置
vector<pair<int, int>> getMoves() {
    vector<pair<int, int>> validMoves;
    // 只考虑已有棋子周围2格内的位置（减少无效计算）
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y)) {
                validMoves.emplace_back(x, y);
            }
        }
    }

    // 若没有有效走法，默认走中心
    if (validMoves.empty()) {
        for (int x = 5; x <= 7; x++) {
            for (int y = 5; y <= 7; y++) {
                if (is_valid_pos(x, y)) {
                    validMoves.emplace_back(x, y);
                }
            }
        }
    }

    // 按评分排序走法（Alpha-Beta剪枝效率更高）
    sort(validMoves.begin(), validMoves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
        return scorePosition(a.first, a.second, my_color) > scorePosition(b.first, b.second, my_color);
        });

    return validMoves;
}

// 打印所有有效候选落子位置，用于验证筛选逻辑
void printValidMoves() {
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
}
// 处理TURN命令
void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves();
    // 检查总时间
    clock_t current_time = clock();
    int used = (current_time - start_total_time) / CLOCKS_PER_SEC;
    if (used > MAX_TOTAL_TIME) {
        debug(("Total time exceeded: " + to_string(used) + "s").c_str());
        return;
    }

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
        update_board(x, y, my_color); // 使用带哈希更新的函数
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