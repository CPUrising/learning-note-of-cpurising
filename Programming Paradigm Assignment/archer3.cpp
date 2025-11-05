#define _CRT_SECURE_NO_WARNINGS

#include <random>                                // C++ 随机数生成库
#include <algorithm>                             // C++ 标准模板库（包含算法函数）
#include <unordered_map>                         // C++ 标准模板库（哈希映射容器）
#include <stdio.h>                               // C 语言标准输入输出库
#include <stdlib.h>                              // C 语言标准通用工具库
#include <string.h>                              // C 语言字符串处理库
#include <time.h>        
#include <string>

// 开局走法结构体，存储坐标信息
struct OpeningMove {
    int xCoordinate;  // x坐标
    int yCoordinate;  // y坐标
    // 默认构造函数
    OpeningMove() : xCoordinate(0), yCoordinate(0) {}
    // 带参数的构造函数
    OpeningMove(int x_, int y_) : xCoordinate(x_), yCoordinate(y_) {}
};

// 开局库，键为走法序列字符串"x1,y1,x2,y2,..."，值为推荐的下一步走法
std::unordered_map<std::string, OpeningMove> openingBook;

/* 类型别名定义 */
using ULL = unsigned long long;  // 64位无符号整数

/* 条件编译 */
#define DEBUG_MODE 0  // 调试模式开关（0为关闭，1为开启）

/* 定义棋子状态类型 */
typedef enum {
    EmptyPosition,  // 空位标记
    BlackPiece,     // 黑棋标记
    WhitePiece      // 白棋标记
} PieceStatus;

/* 定义走法类型 */
typedef struct {
    int xCoordinate;   // x坐标
    int yCoordinate;   // y坐标
    int moveScore;     // 走法评分
} ChessMove;

/* 定义哈希表条目类型 */
typedef struct {
    int searchDepth;  // 搜索深度
    int positionScore;  // 位置评分
} HashTableEntry;

/* 定义常量变量 */
const int OFFSET_X[] = { -1,-1,-1,0,0,1,1,1 };  // X方向偏移量数组
const int OFFSET_Y[] = { -1,0,1,-1,1,-1,0,1 };  // Y方向偏移量数组
const int BOARD_DIMENSION = 12;  // 棋盘尺寸
const int INVALID_POSITION = -1;  // 无效坐标
const int MAX_EVALUATION_SCORE = 1000000000;  // 最高评分
const int MIN_EVALUATION_SCORE = -1000000000;  // 最低评分
const int FIVE_IN_A_LINE_SCORE = 10000000;  // 五子连珠评分
const int LIVE_FOUR_SCORE = 100000;  // 活四评分
const int BLOCKED_FOUR_SCORE = 1000;  // 冲四评分
const int LIVE_THREE_SCORE = 1000;  // 活三评分
const int BLOCKED_THREE_SCORE = 100;  // 冲三评分
const int LIVE_TWO_SCORE = 100;  // 活二评分
const int BLOCKED_TWO_SCORE = 10;  // 冲二评分
const int LIVE_ONE_SCORE = 10;  // 活一评分
const int BLOCKED_ONE_SCORE = 1;  // 冲一评分
const int SEARCH_DEPTH_LEVEL = 3;  // 搜索深度等级

/* 定义全局变量 */
PieceStatus ourPieceColor;  // 我方棋子颜色标记
PieceStatus opponentPieceColor;  // 对手棋子颜色标记
PieceStatus gameBoard[BOARD_DIMENSION][BOARD_DIMENSION];  // 棋盘信息
int ourPieceScores[BOARD_DIMENSION][BOARD_DIMENSION];  // 我方棋子评分
int opponentPieceScores[BOARD_DIMENSION][BOARD_DIMENSION];  // 对手棋子评分
int moveCounter;  // 步数计数器
ULL zobristHashTable[BOARD_DIMENSION][BOARD_DIMENSION][3];  // Zobrist哈希表
ULL currentZobristHashValue;  // 当前Zobrist哈希值
std::unordered_map<ULL, HashTableEntry> transpositionTable;  // Zobrist置换表

/*
 * 函数名称:    isWithinBoardBounds
 * 函数功能:    判断点是否在棋盘范围内
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 * 返回值:      bool - 若在范围内返回true，否则返回false
 */
inline bool isWithinBoardBounds(const int x, const int y)
{
    return (x >= 0 && x < BOARD_DIMENSION && y >= 0 && y < BOARD_DIMENSION);
}

/*
 * 函数名称:    updateZobristHashValue
 * 函数功能:    更新Zobrist哈希值
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const PieceStatus pieceFlag - 棋子状态
 * 返回值:      void
 */
inline void updateZobristHashValue(const int x, const int y, const PieceStatus pieceFlag)
{
    currentZobristHashValue ^= zobristHashTable[x][y][pieceFlag];
}

/*
 * 函数名称:    initOpeningBook
 * 函数功能:    初始化开局库
 * 输入参数:    无
 * 返回值:      void
 */
void initOpeningBook() {
    // 示例1：在中心(5,5)落子后对应(3,3)
    openingBook["5,5"] = OpeningMove(3, 3);
    // 示例2：(5,5)后(3,3)再(7,7)对应(4,6)
    openingBook["5,5,3,3,7,7"] = OpeningMove(4, 6);
    // 示例3：(5,5)后(3,3)再(4,4)对应(6,6)
    openingBook["5,5,3,3,4,4"] = OpeningMove(6, 6);
    // 可根据需要添加更多开局走法...
}

// 走法历史结构体，记录每一步的信息
struct MoveHistoryRecord {
    int xCoordinate;    // x坐标
    int yCoordinate;    // y坐标
    PieceStatus pieceFlag;  // 棋子状态
};

MoveHistoryRecord moveHistoryArray[BOARD_DIMENSION * BOARD_DIMENSION];  // 存储走法历史记录

// 游戏阶段枚举
enum GamePhase {
    OPENING_PHASE,   // 开局阶段（前15步）
    MIDGAME_PHASE,   // 中局阶段（15-30步）
    ENDGAME_PHASE    // 残局阶段（30步后或空位<20）
};

/*
 * 函数名称:    determineGamePhase
 * 函数功能:    判断当前游戏阶段
 * 输入参数:    无
 * 返回值:      GamePhase - 当前游戏阶段
 */
GamePhase determineGamePhase() {
    int emptyPositionCount = 0;
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition) emptyPositionCount++;
        }
    }
    // 残局判断：步数>=30或空位<=20
    if (moveCounter >= 30 || emptyPositionCount <= 20) {
        return ENDGAME_PHASE;
    }
    // 开局判断：步数<=15
    if (moveCounter <= 15) {
        return OPENING_PHASE;
    }
    // 中局
    return MIDGAME_PHASE;
}

/*
 * 函数名称:    hasAdjacentPieces
 * 函数功能:    判断某点周围是否有其他棋子
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const int checkRadius - 检查半径
 *              int requiredCount - 需要的相邻棋子数量
 * 返回值:      bool - 若满足条件返回true，否则返回false
 */
bool hasAdjacentPieces(const int x, const int y, const int checkRadius = 1, int requiredCount = 1)
{
    /* 确定检查范围 */
    int startRow = std::max(0, x - checkRadius);
    int endRow = std::min(BOARD_DIMENSION - 1, x + checkRadius);
    int startCol = std::max(0, y - checkRadius);
    int endCol = std::min(BOARD_DIMENSION - 1, y + checkRadius);

    /* 检查周围是否有足够的相邻棋子 */
    for (int i = startRow; i <= endRow; i++) {
        for (int j = startCol; j <= endCol; j++) {
            if (i == x && j == y) {
                continue;  // 跳过当前点
            }
            if (gameBoard[i][j] != EmptyPosition) {
                if (--requiredCount <= 0) {  // 每找到一个相邻棋子就减少计数
                    return true;
                }
            }
        }
    }
    return false;
}

/*
 * 函数名称:    evaluatePositionState
 * 函数功能:    评估特定棋型状态
 * 输入参数:    const int pieceCount - 连续棋子数量
 *              const int blockCount - 被阻挡数量
 *              const int emptyCount - 空位数量
 * 返回值:      int - 棋型评分
 */
int evaluatePositionState(const int pieceCount, const int blockCount, const int emptyCount)
{
    if (emptyCount <= 0) {
        if (pieceCount >= 5) {
            return FIVE_IN_A_LINE_SCORE;  // 五子连珠
        }
        if (blockCount == 0) {
            switch (pieceCount) {
            case 1: return LIVE_ONE_SCORE;    // 活一
            case 2: return LIVE_TWO_SCORE;    // 活二
            case 3: return LIVE_THREE_SCORE;  // 活三
            case 4: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 1: return BLOCKED_ONE_SCORE;   // 冲一
            case 2: return BLOCKED_TWO_SCORE;   // 冲二
            case 3: return BLOCKED_THREE_SCORE; // 冲三
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            default: break;
            }
        }
    }  // end of if (emptyCount <= 0)
    else if (emptyCount == 1 || emptyCount == pieceCount - 1) {
        if (pieceCount >= 6) {
            return FIVE_IN_A_LINE_SCORE;  // 五子连珠
        }
        if (blockCount == 0) {
            switch (pieceCount) {
            case 2: return LIVE_TWO_SCORE;    // 活二
            case 3: return LIVE_THREE_SCORE;  // 活三
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 2: return BLOCKED_TWO_SCORE;   // 冲二
            case 3: return BLOCKED_THREE_SCORE; // 冲三
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            default: break;
            }
        }
    }  // end of else if (emptyCount == 1 || emptyCount == pieceCount - 1)
    else if (emptyCount == 2 || emptyCount == pieceCount - 2) {
        if (pieceCount >= 7) {
            return FIVE_IN_A_LINE_SCORE;  // 五子连珠
        }
        if (blockCount == 0) {
            switch (pieceCount) {
            case 3: return LIVE_THREE_SCORE;  // 活三
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 3: return BLOCKED_THREE_SCORE; // 冲三
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            default: break;
            }
        }
    }  // end of else if (emptyCount == 2 || emptyCount == pieceCount - 2)
    else if (emptyCount == 3 || emptyCount == pieceCount - 3) {
        if (pieceCount >= 8) {
            return FIVE_IN_A_LINE_SCORE;  // 五子连珠
        }
        if (blockCount == 0) {
            switch (pieceCount) {
            case 4: return LIVE_THREE_SCORE;  // 活三
            case 5: return LIVE_THREE_SCORE;  // 活三
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            case 7: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            case 7: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            case 7: return BLOCKED_FOUR_SCORE;  // 冲四
            default: break;
            }
        }
    }  // end of else if (emptyCount == 3 || emptyCount == pieceCount - 3)
    else if (emptyCount == 4 || emptyCount == pieceCount - 4) {
        if (pieceCount >= 9) {
            return FIVE_IN_A_LINE_SCORE;  // 五子连珠
        }
        if (blockCount == 0) {
            switch (pieceCount) {
            case 5: return LIVE_FOUR_SCORE;   // 活四
            case 6: return LIVE_FOUR_SCORE;   // 活四
            case 7: return LIVE_FOUR_SCORE;   // 活四
            case 8: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;  // 冲四
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            case 7: return BLOCKED_FOUR_SCORE;  // 冲四
            case 8: return LIVE_FOUR_SCORE;   // 活四
            default: break;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 5: return BLOCKED_FOUR_SCORE;  // 冲四
            case 6: return BLOCKED_FOUR_SCORE;  // 冲四
            case 7: return BLOCKED_FOUR_SCORE;  // 冲四
            case 8: return BLOCKED_FOUR_SCORE;  // 冲四
            default: break;
            }
        }
    }  // end of else if (emptyCount == 4 || emptyCount == pieceCount - 4)
    else if (emptyCount == 5 || emptyCount == pieceCount - 5) {
        return FIVE_IN_A_LINE_SCORE;  // 五子连珠
    }  // end of else if (emptyCount == 5 || emptyCount == pieceCount - 5)
    return 0;  // 默认分数
}

/*
 * 函数名称:    evaluateSinglePoint
 * 函数功能:    评估特定点的棋型情况
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const PieceStatus pieceFlag - 棋子状态
 * 返回值:      int - 该点的评分
 */
int evaluateSinglePoint(const int x, const int y, const PieceStatus pieceFlag)
{
    /* 水平方向 ( - ) */
    int totalScore = 0, consecutiveCount = 1, blockCount = 0, emptyPositionCount = -1;
    for (int i = y + 1; true; i++) {
        if (i >= BOARD_DIMENSION) {
            blockCount++;
            break;
        }
        if (gameBoard[x][i] == EmptyPosition) {
            if (emptyPositionCount == -1 && i < BOARD_DIMENSION - 1 && gameBoard[x][i + 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[x][i] == pieceFlag) {
            consecutiveCount++;
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    for (int i = y - 1; true; i--) {
        if (i < 0) {
            blockCount++;
            break;
        }
        if (gameBoard[x][i] == EmptyPosition) {
            if (emptyPositionCount == -1 && i > 0 && gameBoard[x][i - 1] == pieceFlag) {
                emptyPositionCount = 0;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[x][i] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) {
                emptyPositionCount++;
            }
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 垂直方向 ( | ) */
    consecutiveCount = 1;
    blockCount = 0;
    emptyPositionCount = -1;
    for (int i = x + 1; true; i++) {
        if (i >= BOARD_DIMENSION) {
            blockCount++;
            break;
        }
        if (gameBoard[i][y] == EmptyPosition) {
            if (emptyPositionCount == -1 && i < BOARD_DIMENSION - 1 && gameBoard[i + 1][y] == pieceFlag) {
                emptyPositionCount = consecutiveCount;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[i][y] == pieceFlag) {
            consecutiveCount++;
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    for (int i = x - 1; true; i--) {
        if (i < 0) {
            blockCount++;
            break;
        }
        if (gameBoard[i][y] == EmptyPosition) {
            if (emptyPositionCount == -1 && i > 0 && gameBoard[i - 1][y] == pieceFlag) {
                emptyPositionCount = 0;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[i][y] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) {
                emptyPositionCount++;
            }
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 主对角线方向 ( \ ) */
    consecutiveCount = 1;
    blockCount = 0;
    emptyPositionCount = -1;
    for (int i = 1; true; i++) {
        int currentX = x + i;
        int currentY = y + i;
        if (currentX >= BOARD_DIMENSION || currentY >= BOARD_DIMENSION) {
            blockCount++;
            break;
        }
        if (gameBoard[currentX][currentY] == EmptyPosition) {
            if (emptyPositionCount == -1 && (currentX < BOARD_DIMENSION - 1 && currentY < BOARD_DIMENSION - 1) && gameBoard[currentX + 1][currentY + 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[currentX][currentY] == pieceFlag) {
            consecutiveCount++;
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    for (int i = 1; true; i++) {
        int currentX = x - i;
        int currentY = y - i;
        if (currentX < 0 || currentY < 0) {
            blockCount++;
            break;
        }
        if (gameBoard[currentX][currentY] == EmptyPosition) {
            if (emptyPositionCount == -1 && (currentX > 0 && currentY > 0) && gameBoard[currentX - 1][currentY - 1] == pieceFlag) {
                emptyPositionCount = 0;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[currentX][currentY] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) {
                emptyPositionCount++;
            }
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 副对角线方向 ( / ) */
    consecutiveCount = 1;
    blockCount = 0;
    emptyPositionCount = -1;
    for (int i = 1; true; i++) {
        int currentX = x + i;
        int currentY = y - i;
        if (currentX < 0 || currentY < 0 || currentX >= BOARD_DIMENSION || currentY >= BOARD_DIMENSION) {
            blockCount++;
            break;
        }
        if (gameBoard[currentX][currentY] == EmptyPosition) {
            if (emptyPositionCount == -1 && (currentX < BOARD_DIMENSION - 1 && currentY > 0) && gameBoard[currentX + 1][currentY - 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[currentX][currentY] == pieceFlag) {
            consecutiveCount++;
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    for (int i = 1; true; i++) {
        int currentX = x - i;
        int currentY = y + i;
        if (currentX < 0 || currentY < 0 || currentX >= BOARD_DIMENSION || currentY >= BOARD_DIMENSION) {
            blockCount++;
            break;
        }
        if (gameBoard[currentX][currentY] == EmptyPosition) {
            if (emptyPositionCount == -1 && (currentX > 0 && currentY < BOARD_DIMENSION - 1) && gameBoard[currentX - 1][currentY + 1] == pieceFlag) {
                emptyPositionCount = 0;
                continue;
            }
            else {
                break;
            }
        }
        else if (gameBoard[currentX][currentY] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount == -1) {
                emptyPositionCount++;
            }
            continue;
        }
        else {
            blockCount++;
            break;
        }
    }  // end of for
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);
    return totalScore;
}

/*
 * 函数名称:    evaluateOverallBoard
 * 函数功能:    评估我方整体棋盘情况
 * 输入参数:    无
 * 返回值:      int - 棋盘整体评分
 */
int evaluateOverallBoard(void)
{
    int evaluationResult = 0;
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == ourPieceColor) {
                evaluationResult += ourPieceScores[i][j];  // 加上我方分数
            }
            else if (gameBoard[i][j] == opponentPieceColor) {
                evaluationResult -= opponentPieceScores[i][j];  // 减去对手分数
            }
        }
    }
    return evaluationResult;
}

/*
 * 函数名称:    updatePositionScores
 * 函数功能:    更新棋盘评分
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 * 返回值:      void
 */
void updatePositionScores(const int x, const int y)
{
    /* 更新当前点评分 */
    if (gameBoard[x][y] == ourPieceColor) {
        ourPieceScores[x][y] = evaluateSinglePoint(x, y, ourPieceColor);
    }
    else if (gameBoard[x][y] == opponentPieceColor) {
        opponentPieceScores[x][y] = evaluateSinglePoint(x, y, opponentPieceColor);
    }
    else {
        ourPieceScores[x][y] = 0;
        opponentPieceScores[x][y] = 0;
    }

    /* 更新八个方向点的评分 */
    for (int i = 0; i < 8; i++) {
        int newX = x, newY = y;
        while (true) {
            newX += OFFSET_X[i];
            newY += OFFSET_Y[i];
            if (!isWithinBoardBounds(newX, newY)) {
                break;
            }
            if (gameBoard[newX][newY] == ourPieceColor) {
                ourPieceScores[newX][newY] = evaluateSinglePoint(newX, newY, ourPieceColor);
            }
            else if (gameBoard[newX][newY] == opponentPieceColor) {
                opponentPieceScores[newX][newY] = evaluateSinglePoint(newX, newY, opponentPieceColor);
            }
        }
    }
}

/*
 * 函数名称:    makeChessMove
 * 函数功能:    执行一步走棋
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const PieceStatus pieceFlag - 棋子状态（默认空）
 * 返回值:      void
 */
void makeChessMove(const int x, const int y, const PieceStatus pieceFlag = EmptyPosition) {
    updateZobristHashValue(x, y, gameBoard[x][y]);
    gameBoard[x][y] = pieceFlag;
    updateZobristHashValue(x, y, pieceFlag);
    updatePositionScores(x, y);
    // 记录历史（只记录有效走棋）
    if (pieceFlag != EmptyPosition) {
        moveHistoryArray[moveCounter++] = { x, y, pieceFlag };
    }
}

/*
 * 函数名称:    findEmptyPositionToPlace
 * 函数功能:    寻找一个空位落子
 * 输入参数:    const PieceStatus pieceFlag - 棋子状态
 *              int& bestX - 最佳x坐标
 *              int& bestY - 最佳y坐标
 * 返回值:      void
 */
void findEmptyPositionToPlace(const PieceStatus pieceFlag, int& bestX, int& bestY)
{
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition) {
                bestX = i;
                bestY = j;
                return;
            }
        }
    }
}

/*
 * 函数名称:    checkForFiveInARow
 * 函数功能:    检查是否形成五子连珠
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const PieceStatus pieceFlag - 棋子状态
 * 返回值:      bool - 若形成返回true，否则返回false
 */
bool checkForFiveInARow(const int x, const int y, const PieceStatus pieceFlag)
{
    int i, j, consecutiveCount;
    for (int direction = 4; direction < 8; direction++) {
        consecutiveCount = 1;
        i = x + OFFSET_X[direction];
        j = y + OFFSET_Y[direction];
        while (isWithinBoardBounds(i, j) && gameBoard[i][j] == pieceFlag) {
            consecutiveCount++;
            i += OFFSET_X[direction];
            j += OFFSET_Y[direction];
        }
        i = x - OFFSET_X[direction];
        j = y - OFFSET_Y[direction];
        while (isWithinBoardBounds(i, j) && gameBoard[i][j] == pieceFlag) {
            consecutiveCount++;
            i -= OFFSET_X[direction];
            j -= OFFSET_Y[direction];
        }
        if (consecutiveCount >= 5) {
            return true;
        }
    }
    return false;
}

/*
 * 函数名称:    checkLinePattern
 * 函数功能:    检查特定棋型
 * 输入参数:    const int x - x坐标
 *              const int y - y坐标
 *              const PieceStatus pieceFlag - 棋子状态
 *              const int targetCount - 目标棋子数量
 *              const int targetEmpty - 目标空位数
 * 返回值:      bool - 若符合棋型返回true，否则返回false
 */
bool checkLinePattern(const int x, const int y, const PieceStatus pieceFlag, const int targetCount, const int targetEmpty)
{
    int i, j, consecutiveCount, emptyCount;
    for (int direction = 4; direction < 8; direction++) {
        consecutiveCount = 1;
        emptyCount = 0;
        i = x + OFFSET_X[direction];
        j = y + OFFSET_Y[direction];
        while (isWithinBoardBounds(i, j)) {
            if (gameBoard[i][j] == pieceFlag) {
                consecutiveCount++;
            }
            else if (gameBoard[i][j] == EmptyPosition) {
                emptyCount++;
                break;
            }
            else {
                break;
            }
            i += OFFSET_X[direction];
            j += OFFSET_Y[direction];
        }
        i = x - OFFSET_X[direction];
        j = y - OFFSET_Y[direction];
        while (isWithinBoardBounds(i, j)) {
            if (gameBoard[i][j] == pieceFlag) {
                consecutiveCount++;
            }
            else if (gameBoard[i][j] == EmptyPosition) {
                emptyCount++;
                break;
            }
            else {
                break;
            }
            i -= OFFSET_X[direction];
            j -= OFFSET_Y[direction];
        }
        if (consecutiveCount == targetCount && emptyCount == targetEmpty) {
            return true;
        }
    }
    return false;
}

/*
 * 函数名称:    generateCandidateMoves
 * 函数功能:    通过启发式搜索生成候选走法
 * 输入参数:    ChessMove moves[] - 走法数组
 *              int& movesLength - 走法数量
 *              const PieceStatus pieceFlag - 棋子状态
 * 返回值:      void
 */
void generateCandidateMoves(ChessMove moves[], int& movesLength, const PieceStatus pieceFlag)
{
    /* 寻找候选点 */
    movesLength = 0;
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition && hasAdjacentPieces(i, j)) {
                moves[movesLength].xCoordinate = i;
                moves[movesLength].yCoordinate = j;
                moves[movesLength].moveScore = evaluateSinglePoint(i, j, pieceFlag);
                movesLength++;
            }
        }
    }

    /* 启发式排序 */
    std::sort(moves, moves + movesLength, [](const ChessMove& a, const ChessMove& b) {
        return a.moveScore > b.moveScore;
        });
    /* 只保留前6个高分候选点 */
    if (movesLength > 6) {
        movesLength = 6;
    }
}

/*
 * 函数名称:    negamaxWithAlphaBetaPruning
 * 函数功能:    带启发式搜索的负极大值α-β剪枝算法
 * 输入参数:    const int depth - 搜索深度
 *              int alpha - α值
 *              const int beta - β值
 *              const PieceStatus pieceFlag - 棋子状态
 *              int& bestX - 最佳x坐标
 *              int& bestY - 最佳y坐标
 *              bool isTopLevel - 是否顶层调用
 * 返回值:      int - 评分
 */
int negamaxWithAlphaBetaPruning(const int depth, int alpha, const int beta, const PieceStatus pieceFlag, int& bestX, int& bestY, bool isTopLevel = false)
{
    /* 检查置换表 */
    std::unordered_map<ULL, HashTableEntry>::iterator it = transpositionTable.find(currentZobristHashValue);
    if (it != transpositionTable.end() && it->second.searchDepth >= depth) {
        return it->second.positionScore;
    }

    /* 评估棋盘 */
    if (depth == 0) {
        int score = -evaluateOverallBoard();
        transpositionTable[currentZobristHashValue] = { depth, score };
        return score;
    }

    /* 启发式搜索 */
    int currentScore = 0, bestLocalX = INVALID_POSITION, bestLocalY = INVALID_POSITION, movesLength = 0;
    ChessMove nextPossibleMoves[BOARD_DIMENSION * BOARD_DIMENSION];
    generateCandidateMoves(nextPossibleMoves, movesLength, pieceFlag);

    /* 遍历候选点 */
    for (int k = 0; k < movesLength; k++) {
        int i = nextPossibleMoves[k].xCoordinate, j = nextPossibleMoves[k].yCoordinate;
        makeChessMove(i, j, pieceFlag);  // 执行走棋
        currentScore = -negamaxWithAlphaBetaPruning(depth - 1, -beta, -alpha, static_cast<PieceStatus>(3 - pieceFlag), bestX, bestY);
        makeChessMove(i, j);  // 撤销走棋
        if (currentScore >= beta) {
            return beta;  // β剪枝
        }
        if (currentScore > alpha) {
            alpha = currentScore;  // 更新α
            bestLocalX = i;
            bestLocalY = j;
        }
    }

    /* 更新最佳走法 */
    if (isTopLevel && bestLocalX != INVALID_POSITION && bestLocalY != INVALID_POSITION) {
        bestX = bestLocalX;
        bestY = bestLocalY;
    }

    /* 返回最佳评分 */
    return alpha;
}

/*
 * 函数名称:    attemptNextMove
 * 函数功能:    尝试寻找下一步走法
 * 输入参数:    int& nextX - 下一步x坐标
 *              int& nextY - 下一步y坐标
 *              int& score - 评分
 * 返回值:      bool - 若需要进一步搜索返回true，否则返回false
 */
bool attemptNextMove(int& nextX, int& nextY, int& score)
{
    // 首先查询开局库
    std::string moveSequence;
    // 生成当前走法序列字符串（按顺序拼接所有历史坐标）
    for (int step = 0; step < moveCounter; step++) {
        // moveHistoryArray存储历史走棋记录(x,y,flag)
        if (step > 0) moveSequence += ",";
        moveSequence += std::to_string(moveHistoryArray[step].xCoordinate) + "," + std::to_string(moveHistoryArray[step].yCoordinate);
    }

    // 从开局库查找推荐走法
    auto it = openingBook.find(moveSequence);
    if (it != openingBook.end()) {
        nextX = it->second.xCoordinate;
        nextY = it->second.yCoordinate;
        return false;  // 直接使用开局库走法，不需要进一步搜索
    }

    /* 特定步数的默认走法 */
    if (moveCounter == 4) {
        nextX = 4;
        nextY = 4;
        return false;
    }
    if (moveCounter == 5) {
        if (gameBoard[4][7] == BlackPiece) {
            nextX = 7;
            nextY = 4;
            return false;
        }
        if (gameBoard[7][4] == BlackPiece) {
            nextX = 4;
            nextY = 7;
            return false;
        }
        if (gameBoard[4][4] == BlackPiece) {
            nextX = 5;
            nextY = 7;
            return false;
        }
        if (gameBoard[7][7] == BlackPiece) {
            nextX = 6;
            nextY = 4;
            return false;
        }
    }

    /* 检查我方获胜走法（五子连珠） */
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition && checkForFiveInARow(i, j, ourPieceColor)) {
                nextX = i;
                nextY = j;
                return false;
            }
        }
    }

    /* 检查对手获胜走法（五子连珠） */
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition && checkForFiveInARow(i, j, opponentPieceColor)) {
                nextX = i;
                nextY = j;
                return false;
            }
        }
    }

    /* 检查我方优势走法（活四或（冲四且活三）） */
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition && (checkLinePattern(i, j, ourPieceColor, 4, 2) || (checkLinePattern(i, j, ourPieceColor, 4, 1) && checkLinePattern(i, j, ourPieceColor, 3, 2)))) {
                nextX = i;
                nextY = j;
                return false;
            }
        }
    }

    /* 检查对手优势走法（活四或（冲四且活三）） */
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            if (gameBoard[i][j] == EmptyPosition && (checkLinePattern(i, j, opponentPieceColor, 4, 2) || (checkLinePattern(i, j, opponentPieceColor, 4, 1) && checkLinePattern(i, j, opponentPieceColor, 3, 2)))) {
                nextX = i;
                nextY = j;
                return false;
            }
        }
    }

    return true;
}

/*
 * 函数名称:    startGame
 * 函数功能:    开始游戏
 * 输入参数:    无
 * 返回值:      void
 */
void startGame(void)
{
    /* 获取我方棋子颜色 */
    scanf("%d", &ourPieceColor);

    /* 初始化 */
    opponentPieceColor = static_cast<PieceStatus>(3 - ourPieceColor);
    memset(gameBoard, 0, sizeof(gameBoard));
    memset(ourPieceScores, 0, sizeof(ourPieceScores));
    memset(opponentPieceScores, 0, sizeof(opponentPieceScores));

    /* 初始化Zobrist哈希表 */
    std::random_device randomDevice;  // 非确定性随机数生成器
    std::mt19937_64 randomNumberGenerator(randomDevice());  // 基于梅森旋转算法的随机数生成器
    std::uniform_int_distribution<ULL> uniformDistribution;  // 用于生成64位无符号整数的均匀分布
    for (int i = 0; i < BOARD_DIMENSION; i++) {
        for (int j = 0; j < BOARD_DIMENSION; j++) {
            for (int k = 0; k < 3; k++) {
                zobristHashTable[i][j][k] = uniformDistribution(randomNumberGenerator);
            }
        }
    }
    currentZobristHashValue = 0;

    /* 默认初始走法 */
    makeChessMove(5, 5, WhitePiece);
    makeChessMove(6, 6, WhitePiece);
    makeChessMove(5, 6, BlackPiece);
    makeChessMove(6, 5, BlackPiece);
    moveCounter = 4;

    /* 响应终端程序 */
    printf("OK\n");
    fflush(stdout);
}

/*
 * 函数名称:    processOpponentMove
 * 函数功能:    处理对手走棋
 * 输入参数:    无
 * 返回值:      void
 */
void processOpponentMove(void)
{
    ChessMove opponentCommand = { 0, 0, 0 };
    scanf("%d %d", &opponentCommand.xCoordinate, &opponentCommand.yCoordinate);
    makeChessMove(opponentCommand.xCoordinate, opponentCommand.yCoordinate, opponentPieceColor);
    moveCounter++;
}

/*
 * 函数名称:    makeOurMove
 * 函数功能:    执行我方走棋
 * 输入参数:    无
 * 返回值:      void
 */
void makeOurMove(void)
{
    int nextX = INVALID_POSITION, nextY = INVALID_POSITION, score = 0;
    if (attemptNextMove(nextX, nextY, score)) {
        score = negamaxWithAlphaBetaPruning(SEARCH_DEPTH_LEVEL, MIN_EVALUATION_SCORE, MAX_EVALUATION_SCORE, ourPieceColor, nextX, nextY, true);
    }
    if (nextX == INVALID_POSITION || nextY == INVALID_POSITION) {
        findEmptyPositionToPlace(ourPieceColor, nextX, nextY);
    }
    makeChessMove(nextX, nextY, ourPieceColor);
    printf("%d %d\n", nextX, nextY);
    fflush(stdout);
    moveCounter++;
}

/*
 * 函数名称:    main
 * 函数功能:    主函数
 * 返回值:      0
 */
int main()
{
    char commandTag[10] = { 0 };
    while (true) {
        memset(commandTag, 0, sizeof(commandTag));
        scanf("%s", commandTag);
        if (!strcmp(commandTag, "START")) {  // 开始游戏
            startGame();
        }
        else if (!strcmp(commandTag, "PLACE")) {  // 对手走棋
            processOpponentMove();
        }
        else if (!strcmp(commandTag, "TURN")) {  // 我方走棋
            makeOurMove();
        }
        else if (!strcmp(commandTag, "END")) {  // 游戏结束
            int gameStatus;
            scanf("%d", &gameStatus);
            return 0;
        }
    }
}
