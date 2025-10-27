#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>

// 定义长整型别名，用于存储大数值评分
typedef long long LLS;
using namespace std;

// -------------------------- 棋盘与棋子常量定义 --------------------------
#define GRID_DIM 12          // 棋盘维度（12x12网格）
#define BLANK 0              // 空白位置标识（无棋子）
#define PIECE_B 1            // 黑棋标识
#define PIECE_W 2            // 白棋标识
const LLS MAX_VAL = 2000000000000000000LL;  // 评分最大值（用于Alpha-Beta剪枝）
const LLS MIN_VAL = -2000000000000000000LL; // 评分最小值（用于Alpha-Beta剪枝）

// -------------------------- 指令常量定义 --------------------------
#define CMD_INIT "START"       // 初始化指令（告知AI执子颜色）
#define CMD_SET "PLACE"        // 对手落子指令（告知对手落子坐标）
#define CMD_PLAY "TURN"        // AI落子指令（要求AI输出落子坐标）
#define CMD_FINISH "END"       // 游戏结束指令（告知获胜方）
#define CMD_CRACKER "CRACK"    // 调试指令（手动替AI落子）
#define CMD_ERASE "ERASE"      // 调试指令（擦除指定位置棋子）
#define CMD_LIMIT_CHECK "LIMIT"// 调试指令（检查时间限制是否合规）
#define CMD_SHOW_BOARD "SHOW"  // 调试指令（显示当前棋盘状态）
#define CMD_BUGMODE "LIBERTY_STEP" // 调试模式开启指令

// -------------------------- 全局变量 --------------------------
bool bugmode = false;         // 调试模式开关（默认关闭）
clock_t start_turn;           // 记录单次落子开始时间（用于时间限制检查）
int turnround = 0;            // 当前回合数（未实际使用，预留扩展）
int myColor;                  // AI自身的棋子颜色（1=黑，2=白）
int rivalColor;               // 对手的棋子颜色（与AI颜色相反）

// 棋盘中心位置计算（12x12棋盘中心为(5,5)、(5,6)、(6,5)、(6,6)）
const int CENTER_POS_1 = (GRID_DIM + 1) / 2 - 1; // 中心行/列1（5）
const int CENTER_POS_2 = GRID_DIM / 2;           // 中心行/列2（6）
// 四个主要方向向量（下、右、右下、右上），用于检查连子
const int dx[] = { 1, 0, 1, 1 };  // 行方向偏移
const int dy[] = { 0, 1, 1, -1 }; // 列方向偏移

// -------------------------- Zobrist哈希相关定义 --------------------------
#define HASH_SIZE (1 << 20)    // 哈希表大小（2^20 = 1,048,576，平衡空间与冲突）
#define HASH_EXACT 0           // 精确哈希（当前深度下的准确评分）
#define HASH_ALPHA 1           // Alpha哈希（当前评分≤Alpha，无需再探索）
#define HASH_BETA 2            // Beta哈希（当前评分≥Beta，无需再探索）
typedef unsigned long long ULL; // 无符号长整型，用于存储哈希值

// Zobrist哈希表：[棋子类型][行][列] → 随机哈希值
ULL zobristTable[3][GRID_DIM][GRID_DIM];
ULL currentHash;                // 当前棋盘状态的哈希值

// 哈希表条目结构（存储已探索过的棋盘状态，避免重复计算）
struct HashEntry {
    ULL key;    // 棋盘状态的哈希值（键）
    int depth;  // 该状态探索时的深度
    LLS score;  // 该状态的评分
    int type;   // 哈希类型（HASH_EXACT/HASH_ALPHA/HASH_BETA）
};
HashEntry* hashTable;           // 哈希表指针（动态分配内存）

// -------------------------- 其他常量与结构定义 --------------------------
#define MAX_MOVES 121          // 最大落子数（12x12棋盘共144格，预留冗余）

// 棋形结构（未实际使用，预留扩展用于棋形可视化）
struct chessshape {
    int score;       // 棋形评分
    char shape[9];   // 棋形字符串表示（如"11101"代表间隔连子）
};

// 单种棋型的评分结构
struct ChessStyle {
    LLS score;       // 该棋型的基础评分
    // 构造函数（默认评分0）
    ChessStyle(int s = 0) : score(s) {}
};

// 所有棋型的评分配置（AI与对手的棋型权重）
struct AllChessStyle {
    ChessStyle MyStyle[11];    // AI的棋型评分表
    ChessStyle EnemyStyle[11]; // 对手的棋型评分表（与AI权重一致）

    // 构造函数：初始化棋型评分（评分越高，棋型优先级越高）
    AllChessStyle() {
        // 索引0：无有效棋型（评分0）
        MyStyle[0].score = 0;
        // 睡二（两个棋子间隔空白，无直接连子威胁）
        MyStyle[1].score = 10;
        // 活二（两个棋子两端空白，可发展为活三）
        MyStyle[2].score = 100;
        // 睡三（三个棋子间隔空白，需补位才能成四）
        MyStyle[3].score = 100;
        // 活三（三个棋子两端空白，下一步可成活四）
        MyStyle[4].score = 1050;
        // 冲四（四个棋子一端被挡，另一端空白，下一步可成五）
        MyStyle[5].score = 1000;
        // 活四（四个棋子两端空白，下一步必成五）
        MyStyle[6].score = 1100000;
        // 五子连珠（已获胜，最高优先级）
        MyStyle[7].score = 1000000000;
        // 跳活三（两个棋子间隔一格空白，补位后成活三）
        MyStyle[8].score = 900;
        // 跳睡四（三个棋子间隔一格空白，补位后成冲四）
        MyStyle[9].score = 700;
        // 跳活四（三个棋子间隔一格空白，补位后成活四）
        MyStyle[10].score = 1000;

        // 对手的棋型评分与AI一致（STYLE_WEIGHT=1）
        for (int i = 0; i < 11; i++) {
            EnemyStyle[i].score = (LLS)MyStyle[i].score * 1;
        }
    }
};

// 落子位置结构（存储行、列与该位置的评分）
struct Position {
    int row = -1;    // 行坐标（默认-1，代表无效位置）
    int col = -1;    // 列坐标（默认-1，代表无效位置）
    LLS score = 0;   // 该位置的评分（用于排序候选落子）

    // 构造函数（初始化行和列）
    Position(int r = -1, int c = -1) {
        row = r;
        col = c;
    }
};

// 候选落子列表结构（存储所有可能的落子位置）
struct MoveList {
    Position moves[MAX_MOVES]; // 落子位置数组
    int count = 0;             // 有效落子数量（实际候选数）
};

// -------------------------- 工具函数 --------------------------
// 交换两个落子位置（用于排序候选落子）
void swapPositions(Position& pos1, Position& pos2) {
    Position temp = pos1;
    pos1 = pos2;
    pos2 = temp;
}

// 生成64位随机数（用于初始化Zobrist哈希表）
ULL random64() {
    // 组合3个32位随机数，生成64位随机值
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

// -------------------------- Zobrist哈希函数 --------------------------
// 查找哈希表：检查当前棋盘状态是否已探索过
LLS lookupHash(ULL hash, int depth, LLS alpha, LLS beta) {
    // 计算哈希表索引（取模避免越界）
    HashEntry& entry = hashTable[hash % HASH_SIZE];

    // 若找到匹配的哈希值，且探索深度≥当前深度（保证结果可靠）
    if (entry.key == hash && entry.depth >= depth) {
        if (entry.type == HASH_EXACT) {
            return entry.score; // 精确评分，直接返回
        }
        else if (entry.type == HASH_ALPHA && entry.score <= alpha) {
            return alpha;       // 评分≤Alpha，剪枝无需探索
        }
        else if (entry.type == HASH_BETA && entry.score >= beta) {
            return beta;        // 评分≥Beta，剪枝无需探索
        }
    }
    return MIN_VAL - 1; // 未找到匹配，返回无效值
}

// 存储哈希表：将当前棋盘状态的探索结果存入哈希表
void storeHash(ULL hash, int depth, LLS score, int type) {
    // 计算哈希表索引
    HashEntry& entry = hashTable[hash % HASH_SIZE];

    // 仅当当前深度≥已存储深度时更新（保证更深层次的探索结果覆盖浅层）
    if (entry.depth <= depth) {
        entry.key = hash;    // 存储哈希值（键）
        entry.depth = depth; // 存储探索深度
        entry.score = score; // 存储评分
        entry.type = type;   // 存储哈希类型
    }
}

// -------------------------- 棋盘状态管理 --------------------------
// 游戏状态结构（存储棋盘、游戏阶段、哈希值）
struct GameState {
    int grid[GRID_DIM][GRID_DIM] = { { 0 } }; // 12x12棋盘（初始全空白）
    int gamePhase = 0;                        // 游戏阶段（未实际使用，预留扩展）
    ULL hash;                                 // 当前棋盘的哈希值

    // 构造函数：初始化棋盘
    GameState() {
        initializeBoard();
    }

    // 初始化棋盘：清空棋盘并设置初始哈希值
    void initializeBoard() {
        // 清空棋盘（所有位置设为空白）
        memset(grid, 0, sizeof(grid));
        // 初始化哈希值（初始为0）
        hash = 0;
        // 遍历棋盘，计算初始哈希值（若有预放棋子需在此处设置）
        for (int i = 0; i < GRID_DIM; i++) {
            for (int j = 0; j < GRID_DIM; j++) {
                if (grid[i][j] != BLANK) {
                    // 哈希值 = 异或当前位置的Zobrist随机值
                    hash ^= zobristTable[grid[i][j]][i][j];
                }
            }
        }
    }

    // 更新棋盘：落子后更新棋盘状态与哈希值
    void updateGrid(int row, int col, int piece) {
        // 1. 移除原位置棋子的哈希贡献（异或相同值抵消）
        hash ^= zobristTable[grid[row][col]][row][col];
        // 2. 添加新棋子的哈希贡献
        hash ^= zobristTable[piece][row][col];
        // 3. 更新棋盘格子的棋子类型
        grid[row][col] = piece;
    }
};

// 全局棋盘对象（整个程序唯一的棋盘实例）
GameState gameBoard;

// -------------------------- 初始化函数 --------------------------
// 初始化Zobrist哈希表：生成每个(棋子类型, 行, 列)的随机哈希值
void initZobrist() {
    // 设置随机种子（基于当前时间，保证每次运行哈希值不同）
    srand((size_t)time(NULL));
    // 遍历所有棋子类型（0=空白，1=黑，2=白）
    for (int piece = 0; piece < 3; piece++) {
        // 遍历棋盘所有行和列
        for (int i = 0; i < GRID_DIM; i++) {
            for (int j = 0; j < GRID_DIM; j++) {
                // 为每个(棋子, 行, 列)分配64位随机哈希值
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    // 初始化哈希表（动态分配内存）
    hashTable = new HashEntry[HASH_SIZE];
    // 清空哈希表（所有条目初始化为0）
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);

    // 计算初始棋盘的哈希值
    currentHash = 0;
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            if (gameBoard.grid[i][j] != BLANK) {
                currentHash ^= zobristTable[gameBoard.grid[i][j]][i][j];
            }
        }
    }
}

// -------------------------- 评分函数 --------------------------
// 单方向评分：检查某个位置在指定方向上的棋型（如横向、纵向）
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

// 单个位置评分（计算某位置对指定玩家的价值）
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

// 候选位置评分（计算某空白位置的综合价值，含AI与对手的威胁）
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

// -------------------------- 候选落子生成 --------------------------
// 检查空白位置是否有邻子（避免生成无效候选落子，提升效率）
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

// 生成候选落子列表（筛选有价值的落子位置，减少探索量）
void getMoves(GameState* state, struct MoveList* moves, int player) {
    moves->count = 0;      // 重置候选数
    LLS value = 0;         // 单个位置的评分
    int first_zero = 1;    // 标记是否为第一个评分0的位置（避免全空白时无候选）

    // 遍历整个棋盘
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            // 筛选条件：空白位置 + 有邻子（减少无效候选）
            if (state->grid[i][j] == BLANK && hasNexter(state, i, j)) {
                Position tempPos = { i, j }; // 临时位置对象
                value = scorePosts(state, tempPos); // 计算该位置评分

                if (value == 0 && first_zero) {
                    // 情况1：评分0且是第一个 → 保留（避免无候选）
                    moves->moves[moves->count].row = i;
                    moves->moves[moves->count].col = j;
                    moves->moves[moves->count].score = value;
                    moves->count++;
                    first_zero = 0; // 后续评分0的位置不再保留
                }
                else if (value > 0) {
                    // 情况2：评分>0 → 保留（有价值的候选）
                    moves->moves[moves->count].row = i;
                    moves->moves[moves->count].col = j;
                    moves->moves[moves->count].score = value;
                    moves->count++;
                }
            }
        }
    }

    // 候选落子排序：按评分降序（优先探索高价值位置，提升剪枝效率）
    if (moves->count > 1) {
        stable_sort(moves->moves, moves->moves + moves->count,
            [](const Position& a, const Position& b) {
                return a.score > b.score;
            });
    }

    // 限制候选数（最多6个，减少探索量，避免超时）
    if (moves->count > 6) {
        moves->count = 6;
    }
}

// -------------------------- Alpha-Beta剪枝算法 --------------------------
// 棋盘总评分（计算当前棋盘对AI的综合价值）
LLS scoreBoard(GameState* state) {
    LLS comScore = 0;  // AI的总评分
    LLS humScore = 0;  // 对手的总评分

    // 遍历棋盘所有位置
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            Position p = { i, j };
            // 累加AI所有棋子的评分
            if (state->grid[i][j] == myColor) {
                comScore += scorePlayer(state, p, myColor);
            }
            // 累加对手所有棋子的评分
            else if (state->grid[i][j] == rivalColor) {
                humScore += scorePlayer(state, p, rivalColor);
            }
        }
    }

    // 棋盘价值 = AI评分 - 对手评分（差值越大，对AI越有利）
    return comScore - humScore;
}

// Alpha-Beta剪枝核心函数（递归探索未来落子，寻找最优解）
LLS AlphaBeta(GameState* state, int depth, LLS alpha, LLS beta, int player, Position lastMove) {
    // 1. 检查哈希表：若已探索过该状态，直接返回结果（剪枝优化）
    LLS hashValue = lookupHash(state->hash, depth, alpha, beta);
    if (hashValue > MIN_VAL - 1) {
        return hashValue;
    }

    // 2. 检查获胜：若上一步落子导致获胜，返回极端评分（终止递归）
    if (player == myColor && scorePlayer(state, lastMove, rivalColor) >= 1000000000) {
        return MIN_VAL + 1; // 对手上一步获胜，AI得最低分
    }
    else if (player == rivalColor && scorePlayer(state, lastMove, myColor) >= 1000000000) {
        return MAX_VAL - 1; // AI上一步获胜，得最高分
    }

    // 3. 探索深度为0：返回当前棋盘评分（终止递归）
    if (depth == 0) {
        LLS currentScore = scoreBoard(state);
        storeHash(state->hash, depth, currentScore, HASH_EXACT); // 存储哈希
        return currentScore;
    }

    // 4. 生成候选落子列表
    MoveList moves;
    getMoves(state, &moves, player);
    int hashType = HASH_ALPHA; // 初始哈希类型（默认Alpha）

    // 5. 无候选落子：返回当前棋盘评分（平局或棋盘满）
    if (moves.count == 0) {
        LLS currentScore = scoreBoard(state);
        storeHash(state->hash, depth, currentScore, HASH_EXACT);
        return currentScore;
    }

    // 6. 最大化玩家（AI落子，追求最高评分）
    if (player == myColor) {
        LLS maxEval = MIN_VAL; // 初始化最大评分

        // 遍历所有候选落子
        for (int i = 0; i < moves.count; i++) {
            ULL oldHash = state->hash; // 保存当前哈希值（用于回溯）

            // 模拟落子：更新棋盘与哈希
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, myColor);
            // 递归探索：深度-1，切换为对手落子（3-player为对手颜色）
            LLS eval = AlphaBeta(state, depth - 1, alpha, beta, 3 - player, moves.moves[i]);
            // 回溯：恢复棋盘与哈希（不影响后续探索）
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
            state->hash = oldHash;

            // 更新最大评分与Alpha值
            if (eval > maxEval) {
                maxEval = eval;
                if (eval > alpha) {
                    alpha = eval;
                    hashType = HASH_EXACT; // 评分精确，更新哈希类型
                }
            }

            // Beta剪枝：若当前评分≥Beta，对手不会选择此路径，停止探索
            if (beta <= alpha) {
                hashType = HASH_BETA;
                break;
            }
        }

        // 存储当前状态的哈希结果
        storeHash(state->hash, depth, maxEval, hashType);
        return maxEval;
    }
    // 7. 最小化玩家（对手落子，追求最低评分）
    else {
        LLS minEval = MAX_VAL; // 初始化最小评分

        // 遍历所有候选落子
        for (int i = 0; i < moves.count; i++) {
            ULL oldHash = state->hash; // 保存当前哈希值

            // 模拟对手落子
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, rivalColor);
            // 递归探索：深度-1，切换为AI落子
            LLS eval = AlphaBeta(state, depth - 1, alpha, beta, 3 - player, moves.moves[i]);
            // 回溯：恢复棋盘与哈希
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
            state->hash = oldHash;

            // 更新最小评分与Beta值
            if (eval < minEval) {
                minEval = eval;
                if (eval < beta) {
                    beta = eval;
                    hashType = HASH_EXACT; // 评分精确，更新哈希类型
                }
            }

            // Alpha剪枝：若当前评分≤Alpha，AI不会选择此路径，停止探索
            if (beta <= alpha) {
                hashType = HASH_ALPHA;
                break;
            }
        }

        // 存储当前状态的哈希结果
        storeHash(state->hash, depth, minEval, hashType);
        return minEval;
    }
}

// 寻找最优落子（遍历候选落子，调用Alpha-Beta计算最优解）
Position findBestShift(GameState* state) {
    // 清空哈希表（避免上一轮探索结果干扰）
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);

    // 生成AI的候选落子列表
    MoveList moves;
    getMoves(state, &moves, myColor);

    LLS bestScore = MIN_VAL; // 最优评分（初始为最小值）
    Position bestMove(-1, -1); // 最优落子位置（初始为无效）

    // 遍历所有候选落子，计算每个位置的评分
    for (int i = 0; i < moves.count; i++) {
        ULL oldHash = state->hash; // 保存当前哈希值

        // 模拟AI落子
        state->updateGrid(moves.moves[i].row, moves.moves[i].col, myColor);
        // 调用Alpha-Beta计算该落子的后续评分（探索深度MAX_DEPTH=7）
        LLS score = AlphaBeta(state, 7, MIN_VAL, MAX_VAL, rivalColor, moves.moves[i]);
        // 回溯：恢复棋盘与哈希
        state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
        state->hash = oldHash;

        // 更新最优落子（评分更高则替换）
        if (score > bestScore) {
            bestScore = score;
            bestMove = moves.moves[i];
        }
    }

    return bestMove; // 返回最优落子位置
}

// -------------------------- 辅助函数 --------------------------
// 检查落子位置是否有效（在棋盘范围内）
bool isValidMove(Position pos) {
    return pos.row >= 0 && pos.col >= 0 && pos.row < GRID_DIM && pos.col < GRID_DIM;
}

// 打印棋盘（调试模式下可视化棋盘状态）
void printBoard() {
    // 打印列号（0~11）
    printf("    ");
    fflush(stdout); // 强制刷新输出缓冲区（避免延迟）
    for (int col = 0; col < GRID_DIM; col++) {
        printf("%8d ", col);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    // 打印棋盘内容（行号+格子）
    for (int i = 0; i < GRID_DIM; i++) {
        // 打印行分隔线（如"+--------+--------+"）
        printf("   +");
        fflush(stdout);
        for (int j = 0; j < GRID_DIM; j++) {
            printf("--------+");
            fflush(stdout);
        }
        printf("\n%2d ", i); // 打印行号（0~11）
        fflush(stdout);

        // 打印每一行的棋子
        for (int j = 0; j < GRID_DIM; j++) {
            printf("|");
            fflush(stdout);
            if (gameBoard.grid[i][j] == myColor) {
                // 打印AI的棋子（黑棋"●"，白棋"○"）
                printf(myColor == PIECE_B ? "   ●   " : "   ○   ");
                fflush(stdout);
            }
            else if (gameBoard.grid[i][j] == rivalColor) {
                // 打印对手的棋子（与AI相反）
                printf(rivalColor == PIECE_B ? "   ●   " : "   ○   ");
                fflush(stdout);
            }
            else {
                // 空白位置（打印空格）
                printf("        ");
                fflush(stdout);
            }
        }
        printf("|\n"); // 行结束符
        fflush(stdout);
    }

    // 打印棋盘底部分隔线
    printf("   +");
    fflush(stdout);
    for (int j = 0; j < GRID_DIM; j++) {
        printf("--------+");
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

// -------------------------- 游戏主循环（指令处理） --------------------------
void gameLoop() {
    char command[10] = { 0 }; // 存储输入指令（如"START"、"TURN"）
    Position movePos, currentPos; // movePos=对手落子位置，currentPos=当前位置
    Position myturn; // AI的落子位置

    // 无限循环，持续处理指令
    while (1) {
        memset(command, 0, sizeof(command)); // 清空指令缓冲区
        scanf("%s", command); // 读取指令（从标准输入）

        // -------------------------- 处理初始化指令（START） --------------------------
        if (strcmp(command, CMD_INIT) == 0) {
            scanf("%d", &myColor); // 读取AI的棋子颜色（1=黑，2=白）
            rivalColor = 3 - myColor; // 计算对手颜色（与AI相反）
            currentPos.row = CENTER_POS_1; // 初始位置设为中心（预留）
            currentPos.col = (myColor == PIECE_B) ? CENTER_POS_2 : CENTER_POS_1;
            gameBoard.initializeBoard(); // 初始化棋盘

            // 响应指令：输出"OK"（必须在1秒内响应）
            printf("OK\n");
            fflush(stdout);

            // 调试模式：打印初始棋盘
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- 处理调试模式开启指令（LIBERTY_STEP） --------------------------
        else if (strcmp(command, CMD_BUGMODE) == 0) {
            bugmode = true; // 开启调试模式
            // 打印调试模式欢迎信息
            printf("\n\n\n\n\n\n");
            printf("     WELCOME TO LIBERTY     \n");
            printf("\n\n\n\n\n\n");
        }

        // -------------------------- 处理对手落子指令（PLACE） --------------------------
        else if (strcmp(command, CMD_SET) == 0) {
            scanf("%d %d", &movePos.row, &movePos.col); // 读取对手落子坐标
            // 更新棋盘：在对手落子位置放置对手棋子
            gameBoard.updateGrid(movePos.row, movePos.col, rivalColor);

            // 调试模式：打印更新后的棋盘
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- 处理调试手动落子指令（CRACK） --------------------------
        else if (strcmp(command, CMD_CRACKER) == 0 && bugmode) {
            scanf("%d %d", &movePos.row, &movePos.col); // 读取手动落子坐标
            // 更新棋盘：在指定位置放置AI棋子
            gameBoard.updateGrid(movePos.row, movePos.col, myColor);

            // 调试模式：打印更新后的棋盘
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- 处理调试擦除指令（ERASE） --------------------------
        else if (strcmp(command, CMD_ERASE) == 0 && bugmode) {
            scanf("%d %d", &movePos.row, &movePos.col); // 读取擦除位置坐标
            // 更新棋盘：将指定位置设为空白
            gameBoard.updateGrid(movePos.row, movePos.col, BLANK);

            // 调试模式：打印更新后的棋盘
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- 处理AI落子指令（TURN） --------------------------
        else if (strcmp(command, CMD_PLAY) == 0) {
            start_turn = clock(); // 记录落子开始时间（用于时间限制检查）
            Position nextMove; // 存储AI的最优落子位置

            // 调用函数寻找最优落子
            nextMove = findBestShift(&gameBoard);

            // 若落子位置有效，输出坐标并更新棋盘
            if (isValidMove(nextMove)) {
                printf("%d %d\n", nextMove.row, nextMove.col); // 输出落子坐标
                fflush(stdout); // 强制刷新（避免超时）
                // 更新棋盘：放置AI的棋子
                gameBoard.updateGrid(nextMove.row, nextMove.col, myColor);

                // 调试模式：打印更新后的棋盘
                if (bugmode) {
                    printBoard();
                }
            }
        }

        // -------------------------- 处理调试显示棋盘指令（SHOW） --------------------------
        else if (strcmp(command, CMD_SHOW_BOARD) == 0 && bugmode) {
            printBoard(); // 打印当前棋盘
        }

        // -------------------------- 处理调试时间检查指令（LIMIT） --------------------------
        else if (strcmp(command, CMD_LIMIT_CHECK) == 0 && bugmode) {
            bool failed = false; // 时间检查是否失败（超时）

            // 模拟多回合落子，检查时间限制（1900ms/步）
            for (int i = 0; i < MAX_MOVES; i++) {
                auto start1 = clock(); // 记录开始时间
                Position nextMove = findBestShift(&gameBoard); // 寻找最优落子

                // 若落子有效，更新棋盘
                if (isValidMove(nextMove)) {
                    fflush(stdout);
                    gameBoard.updateGrid(nextMove.row, nextMove.col, myColor);
                }

                // 计算耗时（毫秒）
                auto finish = clock();
                auto duration = (double)(finish - start1);

                // 若耗时超过1900ms，标记失败并退出循环
                if (duration > 1900) {
                    failed = true;
                    break;
                }

                swap(myColor, rivalColor); // 切换颜色（模拟对手落子）
            }

            // 输出时间检查结果
            if (failed) {
                printf("\n\n\n\n\n\n");
                printf("     TIME CHECK FAILED     \n");
                printf("\n\n\n\n\n\n");
            }
            else {
                printf("\n\n\n\n\n\n");
                printf("     TIME CHECK SUCCESSED     \n");
                printf("\n\n\n\n\n\n");
            }
        }
    }
}

// -------------------------- 主函数（程序入口） --------------------------
int main() {
    initZobrist(); // 初始化Zobrist哈希表
    gameLoop();    // 启动游戏主循环（处理指令）
    return 0;      // 程序正常结束
}