#include "constants.h"
#include "structures.h"
#include "hash.cpp"
#include <algorithm>
#include <cstring>
#include <ctime>

// 全局变量
bool bugmode = false;
int turnround = 0;
int myColor;
int rivalColor;
clock_t start_turn;
const int CENTER_POS_1 = (GRID_DIM + 1) / 2 - 1;
const int CENTER_POS_2 = GRID_DIM / 2;
const int dx[] = { 1, 0, 1, 1 };
const int dy[] = { 0, 1, 1, -1 };
GameState gameBoard;

// 实现结构体方法
AllChessStyle::AllChessStyle() {
    MyStyle[0].score = 0;
    MyStyle[SLEEPING2].score = 10;
    MyStyle[LIVE2].score = 100;
    MyStyle[SLEEPING3].score = 100;
    MyStyle[LIVE3].score = 1050;
    MyStyle[RUSH4].score = 1000;
    MyStyle[LIVE4].score = 1100000;
    MyStyle[FIVE5].score = 1000000000;
    MyStyle[JUMPLIVE3].score = 900;
    MyStyle[JUMPSLEEPING4].score = 700;
    MyStyle[JUMPLIVE4].score = 1000;

    for (int i = 0; i < STYLENUM; i++) {
        EnemyStyle[i].score = LLS(MyStyle[i].score);
    }
}

GameState::GameState() {
    initializeBoard();
}

void GameState::initializeBoard() {
    memset(grid, 0, sizeof(grid));
    grid[CENTER_POS_1][CENTER_POS_1] = PIECE_W;
    grid[CENTER_POS_2][CENTER_POS_2] = PIECE_W;
    grid[CENTER_POS_2][CENTER_POS_1] = PIECE_B;
    grid[CENTER_POS_1][CENTER_POS_2] = PIECE_B;

    hash = 0;
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            if (grid[i][j] != BLANK) {
                hash ^= zobristTable[grid[i][j]][i][j];
            }
        }
    }
}

void GameState::updateGrid(int row, int col, int piece) {
    hash ^= zobristTable[grid[row][col]][row][col];
    hash ^= zobristTable[piece][row][col];
    grid[row][col] = piece;
}

// 工具函数
void swapPositions(Position& pos1, Position& pos2) {
    Position temp = pos1;
    pos1 = pos2;
    pos2 = temp;
}

bool isEven2(int number) {
    return number % 2 == 0;
}

// 评分函数
int scoreDirects(GameState* state, Position pos, int dx, int dy, int player) {
    int count = 1;          // 当前连子数（初始为1，即当前位置的棋子）
    int empty_pos = 0;      // 空白位置状态（1=单端空白，2=两端空白）
    int empty_count = 0;    // 空白位置数量（未实际使用）
    int block = 0;          // 被阻挡次数（两端被对手棋子阻挡的次数）
    // 跳跃连子相关变量（处理间隔一格的连子，如"101"）
    int jump_p = 0, jump_n = 0;        // 正/反方向是否存在跳跃
    int count_j_p = 0, count_j_n = 0;  // 正/反方向跳跃连子数
    bool block_j_p = 0, block_j_n = 0; // 正/反方向跳跃后是否被挡
    bool empty_j_p = 0, empty_j_n = 0; // 正/反方向跳跃后是否空白

    // -------------------------- 正方向检查（如向右、向下） --------------------------
    int tx = pos.row + dx;  // 当前行 + 方向偏移（正方向）
    int ty = pos.col + dy;  // 当前列 + 方向偏移（正方向）
    bool found_empty = false; // 是否找到空白位置（用于判断跳跃）

    // 循环探索正方向，直到超出棋盘边界
    while (tx >= 0 && tx < GRID_DIM && ty >= 0 && ty < GRID_DIM) {
        if (found_empty && !jump_p) {
            // 情况1：已找到空白，且未标记跳跃 → 检查是否为跳跃连子
            if (state->grid[tx][ty] == player) {
                jump_p = 1;       // 标记为跳跃连子
                count_j_p++;      // 跳跃连子数+1
            }
            else {
                break;            // 非己方棋子，停止探索
            }
        }
        else if (found_empty && jump_p) {
            // 情况2：已找到空白且标记跳跃 → 继续检查跳跃后的连子
            if (state->grid[tx][ty] == player) {
                count_j_p++;      // 跳跃连子数+1
            }
            else if (state->grid[tx][ty] == 3 - player) {
                block_j_p = true; // 被对手棋子阻挡
                break;
            }
            else {
                empty_j_p = true; // 跳跃后为空白
                break;
            }
        }
        else {
            // 情况3：未找到空白 → 检查连续连子
            if (state->grid[tx][ty] == BLANK) {
                empty_count++;    // 空白数+1
                empty_pos += 1;   // 标记单端空白
                found_empty = true; // 标记已找到空白
            }
            else if (state->grid[tx][ty] != player) {
                block++;          // 被对手棋子阻挡
                break;
            }
            else {
                count++;          // 己方连续连子数+1
            }
        }
        // 移动到下一个位置（正方向）
        tx += dx;
        ty += dy;
    }

    // -------------------------- 反方向检查（如向左、向上） --------------------------
    tx = pos.row - dx;  // 当前行 - 方向偏移（反方向）
    ty = pos.col - dy;  // 当前列 - 方向偏移（反方向）
    found_empty = false; // 重置空白标记

    // 循环探索反方向，直到超出棋盘边界
    while (tx >= 0 && tx < GRID_DIM && ty >= 0 && ty < GRID_DIM) {
        if (found_empty && !jump_n) {
            // 情况1：已找到空白，且未标记跳跃 → 检查跳跃连子
            if (state->grid[tx][ty] == player) {
                jump_n = 1;       // 标记为跳跃连子
                count_j_n++;      // 跳跃连子数+1
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_n) {
            // 情况2：已找到空白且标记跳跃 → 继续检查跳跃后的连子
            if (state->grid[tx][ty] == player) {
                count_j_n++;      // 跳跃连子数+1
            }
            else if (state->grid[tx][ty] == 3 - player) {
                block_j_n = true; // 被对手阻挡
                break;
            }
            else {
                empty_j_n = true; // 跳跃后为空白
                break;
            }
        }
        else {
            // 情况3：未找到空白 → 检查连续连子
            if (state->grid[tx][ty] == BLANK) {
                empty_count++;    // 空白数+1
                empty_pos += 2;   // 标记两端空白（正方向已算1，此处+2 → 总3）
                found_empty = true; // 标记已找到空白
            }
            else if (state->grid[tx][ty] != player) {
                block++;          // 被对手阻挡
                break;
            }
            else {
                count++;          // 己方连续连子数+1
            }
        }
        // 移动到下一个位置（反方向）
        tx -= dx;
        ty -= dy;
    }

    // -------------------------- 棋型判断与评分 --------------------------
    // 处理跳跃连子（如跳活三、跳活四）
    if (jump_p && jump_n) {
        // 双向跳跃（未实现，预留扩展）
    }
    else if (jump_p) {
        // 正方向跳跃
        if (count + count_j_p == 3 && empty_pos == 3 && empty_j_p) {
            return 8; // 跳活三
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && block_j_p) {
            return 9; // 跳睡四
        }
        else if (count + count_j_p == 4 && empty_pos == 1 && empty_j_p) {
            return 9; // 跳睡四
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && empty_j_p) {
            return 10; // 跳活四
        }
    }
    else if (jump_n) {
        // 反方向跳跃
        if (count + count_j_n == 3 && empty_pos == 3 && empty_j_n) {
            return 8; // 跳活三
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && block_j_n) {
            return 9; // 跳睡四
        }
        else if (count + count_j_n == 4 && empty_pos == 2 && empty_j_n) {
            return 9; // 跳睡四
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && empty_j_n) {
            return 10; // 跳活四
        }
    }

    // 处理普通连子（无跳跃）
    if (count >= 5) {
        return 7;  // 五子连珠（获胜）
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return 6; // 活四（两端空白，必赢）
        }
        else if (empty_pos > 0) {
            return 5; // 冲四（一端空白，需补位）
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return 4; // 活三（两端空白，可发展为活四）
        }
        else if (empty_pos > 0) {
            return 3; // 睡三（一端空白，需补位才能冲四）
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return 2; // 活二（两端空白，可发展为活三）
        }
        else if (empty_pos > 0) {
            return 1; // 睡二（一端空白，需补位才能发展）
        }
    }

    return 0; // 无有效棋型，评分0
}

LLS scorePlayer(GameState* state, Position pos, int player) {
    AllChessStyle styles;  // 棋型评分配置
    LLS totalScore = 0;    // 总评分

    // 检查四个主要方向（下、右、右下、右上）的棋型
    for (int i = 0; i < 4; i++) {
        // 获取当前方向的棋型类型（如活三、冲四）
        int styleType = scoreDirects(state, pos, dx[i], dy[i], player);
        // 累加该棋型的评分
        totalScore += styles.MyStyle[styleType].score;
    }

    // 评分修正：若评分在1400~1e6之间，强制设为1e6（提升中高威胁棋型的优先级）
    if (totalScore >= 1400 && totalScore < 1000000) {
        totalScore = 1000000;
    }

    return totalScore;
}

LLS scorePosts(GameState* state, Position pos) {
    AllChessStyle styles;  // 棋型评分配置
    LLS totalScore = 0;    // 总评分
    LLS myScore = 0;       // AI在该位置的评分
    LLS rivalScore = 0;    // 对手在该位置的评分

    // 检查四个主要方向的棋型
    for (int i = 0; i < 4; i++) {
        // 计算AI在该方向的棋型评分
        int myStyle = scoreDirects(state, pos, dx[i], dy[i], myColor);
        // 计算对手在该方向的棋型评分
        int rivalStyle = scoreDirects(state, pos, dx[i], dy[i], rivalColor);

        myScore += styles.MyStyle[myStyle].score;
        rivalScore += styles.EnemyStyle[rivalStyle].score;
    }

    // 评分修正（同scorePlayer函数，提升中高威胁优先级）
    if (rivalScore >= 1400 && rivalScore < 1000000) {
        rivalScore = 1000000;
    }
    if (myScore >= 1400 && myScore < 1000000) {
        myScore = 1000000;
    }

    // 综合评分 = AI评分 + 对手威胁评分（优先防守对手高威胁）
    totalScore += myScore + rivalScore;
    return totalScore;
}

// 移动生成与排序
int hasNexter(GameState* state, int row, int col) {
    // 8个方向（上下左右、四个对角线），检查2格范围内是否有棋子
    const int directions[8][2] = {
        { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 },
        { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, 1 }
    };

    for (int d = 0; d < 8; d++) {
        // 检查1~2格范围内的位置
        for (int distance = 1; distance <= 2; distance++) {
            int ni = row + directions[d][0] * distance; // 目标行
            int nj = col + directions[d][1] * distance; // 目标列

            // 若目标位置在棋盘内且有棋子（非空白）
            if (ni >= 0 && ni < GRID_DIM && nj >= 0 && nj < GRID_DIM && state->grid[ni][nj] != BLANK) {
                return 1; // 有邻子，该位置为有效候选
            }
        }
    }
    return 0; // 无邻子，排除该候选
}

int partitionMoves(Position moveArr[], int startIdx, int endIdx, GameState* state, int player) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}

void quickSortMoves(Position moveArr[], int startIdx, int endIdx, GameState* state, int player) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}

bool comparePositions(const Position& a, const Position& b) {
    return a.score > b.score;
}

void getMoves(GameState* state, MoveList* moves, int player) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}

// AI搜索算法
LLS scoreBoard(GameState* state) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}

LLS AlphaBeta(GameState* state, int depth, LLS alpha, LLS beta, int player, Position lastMove) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}

Position findBestShift(GameState* state) {
    // 实现原代码中的逻辑
    // ...（省略具体实现，保持与原代码一致）
}