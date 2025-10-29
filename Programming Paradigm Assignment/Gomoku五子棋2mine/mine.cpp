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
vector<pair<int, int>> getMoves();

#define DEBUG_MODE 1

const int BOARD_SIZE = 12;    // 扩大棋盘尺寸以适应更多战术
const int BLACK = 1;
const int WHITE = 2;
const int EMPTY = 0;
const int MAX_SINGLE_TIME = 2;
const int MAX_TOTAL_TIME = 90;
const int HASH_SIZE = 1000003;
using ULL = unsigned long long;
using LLS = long long;

// 调整分数权重，增强威胁识别优先级
const int SCORE_FIVE = 1000000;
const int SCORE_LIVE_FOUR = 100000;
const int SCORE_JUMP_LIVE_FOUR = 90000;
const int SCORE_RUSH_FOUR = 20000;  // 提高冲四分数
const int SCORE_JUMP_RUSH_FOUR = 18000;
const int SCORE_LIVE_THREE = 5000;  // 提高活三分数
const int SCORE_JUMP_LIVE_THREE = 4500;
const int SCORE_SLEEP_THREE = 1000;
const int SCORE_LIVE_TWO = 100;
const int SCORE_SLEEP_TWO = 50;

enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

struct HashEntry {
    ULL key;
    int depth;
    LLS score;
    HashType type;
};

vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;
clock_t start_total_time;
ULL zobristTable[3][BOARD_SIZE][BOARD_SIZE];
HashEntry* hashTable;
ULL currentHash = 0;
int threat_count = 0;  // 新增威胁计数器

ULL random64() {
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

void initZobrist() {
    srand((unsigned int)time(NULL));
    for (int piece = 0; piece < 3; piece++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

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

void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

void debug(const char* message) {
#if DEBUG_MODE == 1
    printf("DEBUG %s\n", message);
    fflush(stdout);
#endif
}

void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    currentHash = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            currentHash ^= zobristTable[EMPTY][i][j];
        }
    }
    debug("Board initialized successfully");
}

bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;
    }
    return board[x][y] == EMPTY;
}

void update_board(int x, int y, int color) {
    currentHash ^= zobristTable[EMPTY][x][y];
    currentHash ^= zobristTable[color][x][y];
    board[x][y] = color;
}

// 新增：检查是否形成连四（用于威胁评估）
bool is_four_in_row(int x, int y, int color) {
    int dirs[4][2] = { {0,1},{1,0},{1,1},{1,-1} };
    for (auto& dir : dirs) {
        int count = 1;
        // 正向计数
        for (int i = 1; i <= 4; i++) {  // 改为<=4，确保能检测到连续4子的情况
            int nx = x + dir[0] * i;
            int ny = y + dir[1] * i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[nx][ny] != color)
                break;
            count++;
        }
        // 反向计数
        for (int i = 1; i <= 4; i++) {  // 同上
            int nx = x - dir[0] * i;
            int ny = y - dir[1] * i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[nx][ny] != color)
                break;
            count++;
        }
        // 当存在连续4子时即判定为冲四威胁
        if (count == 4) {
            // 检查两端是否有空位（确保是冲四而不是死四）
            int end1_x = x + dir[0] * (count - 1);
            int end1_y = y + dir[1] * (count - 1);
            int end2_x = x - dir[0] * (count - 1);
            int end2_y = y - dir[1] * (count - 1);
            bool has_empty = false;
            if (is_valid_pos(end1_x + dir[0], end1_y + dir[1])) has_empty = true;
            if (is_valid_pos(end2_x - dir[0], end2_y - dir[1])) has_empty = true;
            if (has_empty) return true;
        }
        // 已有五子直接判定胜利
        if (count >= 5) return true;
    }
    return false;
}

int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;  // 对手颜色
    int count = 1;          // 连续的己方棋子数（初始为1，包含当前位置）
    int empty_pos = 0;      // 记录方向上的空位状态（用于判断“活”/“眠”）
    int block = 0;          // 记录方向上的阻挡数（对手棋子或边界）

    // 跳棋形相关变量（处理非连续连子，如“跳活三”“跳冲四”）
    int jump_p = 0, jump_n = 0;
    int count_j_p = 0, count_j_n = 0;
    bool block_j_p = false, block_j_n = false;
    bool empty_j_p = false, empty_j_n = false;

    // -------------------------- 1. 正向方向搜索（从当前位置向(dx, dy)延伸）--------------------------
    int x = posx + dx;  // 从当前位置的下一个格子开始搜索
    int y = posy + dy;
    bool found_empty = false;  // 标记是否遇到过空位

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {  // 不超出棋盘边界
        if (found_empty && !jump_p) {
            // 情况1：已遇到空位，且未触发“跳棋形”——若再遇己方棋子，判定为“跳棋”
            if (board[x][y] == player) {
                jump_p = 1;          // 标记正向存在跳棋
                count_j_p++;         // 跳棋的连续子数+1
            }
            else {
                break;  // 遇到对手棋子或边界，停止搜索
            }
        }
        else if (found_empty && jump_p) {
            // 情况2：已触发“跳棋形”——继续判断跳棋的延伸状态
            if (board[x][y] == player) {
                count_j_p++;         // 跳棋连子数继续+1
            }
            else if (board[x][y] == opponent) {
                block_j_p = true;    // 跳棋被对手阻挡
                break;
            }
            else {
                empty_j_p = true;    // 跳棋末端有空位（可延伸）
                break;
            }
        }
        else {
            // 情况3：未遇到空位——正常连续子搜索
            if (board[x][y] == EMPTY) {
                empty_pos += 1;      // 记录空位（正向空位标记为1）
                found_empty = true;  // 标记已遇到空位
            }
            else if (board[x][y] == opponent) {
                block++;             // 遇到对手棋子，阻挡数+1
                break;
            }
            else {
                count++;             // 遇到己方棋子，连续子数+1
            }
        }
        x += dx;  // 继续向正向移动
        y += dy;
    }

    // -------------------------- 2. 反向方向搜索（从当前位置向(-dx, -dy)延伸）--------------------------
    x = posx - dx;  // 从当前位置的前一个格子开始搜索
    y = posy - dy;
    found_empty = false;  // 重置空位标记

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_n) {
            // 与正向逻辑一致，处理反向的“跳棋形”
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
            // 未遇到空位的正常搜索
            if (board[x][y] == EMPTY) {
                empty_pos += 2;      // 反向空位标记为2（与正向区分，用于后续判断）
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;
            }
        }
        x -= dx;  // 继续向反向移动
        y -= dy;
    }

    // -------------------------- 3. 跳棋形评分（处理非连续连子）--------------------------
    if (jump_p || jump_n) {
        int total_jump = count + max(count_j_p, count_j_n);  // 总连子数（基础连子+跳棋连子）
        // 跳活四（如“○○□○○”，中间空一格但两端可延伸）
        if (total_jump == 4) {
            if ((jump_p && empty_j_p && empty_pos == 3) || (jump_n && empty_j_n && empty_pos == 3)) {
                return SCORE_JUMP_LIVE_FOUR;  // 跳活四分数（高优先级）
            }
            else if ((jump_p && (block_j_p || empty_j_p)) || (jump_n && (block_j_n || empty_j_n))) {
                return SCORE_JUMP_RUSH_FOUR;  // 跳冲四分数（中高优先级）
            }
        }
        // 跳活三（如“○□○○”，中间空一格但可形成活三）
        else if (total_jump == 3 && empty_pos == 3) {
            return SCORE_JUMP_LIVE_THREE;  // 跳活三分数（中优先级）
        }
    }

    // -------------------------- 4. 普通连续棋形评分（处理连续连子）--------------------------
    if (count >= 5) {
        return SCORE_FIVE;  // 连五（直接获胜，最高优先级）
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR;  // 活四（两端均有空位，可直接冲五，高优先级）
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR;  // 冲四（一端有空位，需补一子成五，中高优先级）
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE;  // 活三（两端有空位，可发展为活四，中优先级）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE;  // 眠三（一端被挡，发展受限，低优先级）
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO;  // 活二（两端有空位，可发展为活三，低优先级）
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO;  // 眠二（一端被挡，发展受限，极低优先级）
        }
    }

    return 0;  // 无有效棋形（如单子、死子，无战术价值）
}

LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // 检查是否形成四连子威胁
    bool is_four = is_four_in_row(x, y, player);
    if (is_four) {
        total += SCORE_RUSH_FOUR * 0.5;  // 额外加分
    }

    for (auto& dir : dirs) {
        total += scoreDirects(x, y, dir[0], dir[1], player);
    }

    for (auto& dir : dirs) {
        total -= scoreDirects(x, y, dir[0], dir[1], opponent) * 0.9;
    }

    update_board(x, y, EMPTY);
    return total;
}

bool check_win(int x, int y, int color) {
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1;
        //正向搜索
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

// 新增：统计当前威胁数量
void count_threats() {
    threat_count = 0;
    int enemy = (my_color == BLACK) ? WHITE : BLACK;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == enemy) {
                if (is_four_in_row(i, j, enemy)) {
                    threat_count++;
                }
            }
        }
    }
}

LLS evaluate() {
    LLS totalScore = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                totalScore -= scorePosition(i, j, board[i][j]) * 0.9;
            }
        }
    }
    return totalScore;
}

LLS alphaBeta(int depth, LLS alpha, LLS beta, int max_depth) {
    // 限制最大递归深度，防止超时
    if (depth >= max_depth) {
        LLS score = evaluate();
        return score;
    }

    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    vector<pair<int, int>> moves = getMoves();  // 使用优化后的候选集
    if (moves.empty()) {
        LLS score = 0;
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    LLS bestScore = -1e18;
    int currentColor = (depth % 2 == 0) ? my_color : (my_color == BLACK ? WHITE : BLACK);

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        update_board(x, y, currentColor);
        bool isWin = check_win(x, y, currentColor);

        LLS score;
        if (isWin) {
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            score = -alphaBeta(depth - 1, -beta, -alpha, max_depth);
        }

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
        }
        if (bestScore > alpha) {
            alpha = bestScore;
        }
        if (alpha >= beta) {
            break;
        }
    }

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
    count_threats();  // 先统计威胁数量
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;

    // 1. 优先防守能直接获胜的位置
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY);//to understand
                
                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 2. 防守冲四和跳冲四威胁（增强版）
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                LLS enemy_score = 0;
                int dirs[4][2] = { {0,1},{1,0},{1,1},{1,-1} };
                for (auto& dir : dirs) {
                    enemy_score += scoreDirects(x, y, dir[0], dir[1], enemy_color);
                }
                // 检查是否形成四连威胁,这TM没有用吧
                if (is_four_in_row(x, y, enemy_color)) {
                    enemy_score += SCORE_RUSH_FOUR;
                }
                update_board(x, y, EMPTY);

                if (enemy_score >= SCORE_RUSH_FOUR) {
                    debug(("Block enemy rush four at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 3. 根据威胁程度动态调整搜索深度
    int searchDepth = 4;
    if (threat_count >= 2) {
        searchDepth = 6;  // 多威胁时增加搜索深度
        debug("Increased search depth due to multiple threats");
    }

    // 4. 带深度限制的Alpha-Beta搜索
    int bestX = -1, bestY = -1;
    LLS bestScore = -1e18;
    vector<pair<int, int>> moves = getMoves();

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        update_board(x, y, my_color);
        bool isWin = check_win(x, y, my_color);
        LLS score;

        if (isWin) {
            score = 1e18;
        }
        else {
            score = -alphaBeta(searchDepth - 1, -1e18, 1e18, searchDepth);
        }

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    if (bestX != -1) {
        debug(("Hash-optimized move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback逻辑保持不变
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 1)) {
                debug(("Near enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    for (int x = 7; x <= 8; x++) {  // 中心区域调整为棋盘中心
        for (int y = 7; y <= 8; y++) {
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

void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock();
    debug(("Start game with color: " + to_string(my_color)).c_str());
    printf("OK\n");
    fflush(stdout);
}

void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        update_board(x, y, enemy_color);
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

bool hasNeighbor(int x, int y, int distance) {
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            if (dx == 0 && dy == 0) continue;

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

int getDynamicDepth(clock_t start_single_time) {
    clock_t current = clock();
    double elapsed = (current - start_single_time) / (double)CLOCKS_PER_SEC;
    int base_depth = 3;

    // 根据剩余时间动态调整深度
    if (elapsed < 0.5) {
        return (threat_count >= 2) ? 5 : 4;
    }
    else if (elapsed < 1.0) {
        return (threat_count >= 2) ? 4 : 3;
    }
    else {
        return 2; // 时间紧张时使用浅度搜索
    }
}
vector<pair<int, int>> getMoves() {
    vector<pair<int, int>> validMoves;
    // 扩大邻居检测范围到3格，减少漏检
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 2)) {
                validMoves.emplace_back(x, y);
            }
        }
    }

    if (validMoves.empty()) {
        for (int x = 7; x <= 8; x++) {
            for (int y = 7; y <= 8; y++) {
                if (is_valid_pos(x, y)) {
                    validMoves.emplace_back(x, y);
                }
            }
        }
    }

    // 优化排序，结合防守权重
    sort(validMoves.begin(), validMoves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
        int enemy = (my_color == BLACK) ? WHITE : BLACK;
        LLS scoreA = scorePosition(a.first, a.second, my_color) +
            scorePosition(a.first, a.second, enemy) * 0.5;  // 增加防守权重
        LLS scoreB = scorePosition(b.first, b.second, my_color) +
            scorePosition(b.first, b.second, enemy) * 0.5;
        return scoreA > scoreB;
        });
    if (validMoves.size() > 20) {
        validMoves.resize(20);
    }
    return validMoves;
}

void printValidMoves() {
#if DEBUG_MODE == 1
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
#endif
}

void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves();

    // 只获取一次最佳走法
    pair<int, int> best_move = get_best_move();
    int x = best_move.first, y = best_move.second;

    // 检查时间是否超时
    double single_used = (clock() - start_single_time) / (double)CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return;
    }

    if (is_valid_pos(x, y)) {
        update_board(x, y, my_color);
        printf("%d %d\n", x, y);
        fflush(stdout);
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        //  fallback逻辑
        debug("Invalid move generated!");
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
    initZobrist();
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

    delete[] hashTable;
    return 0;
}