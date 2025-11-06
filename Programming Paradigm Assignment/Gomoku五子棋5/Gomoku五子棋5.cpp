#ifdef _MSC_VER
#define strdup _strdup
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <limits.h>

#ifdef _MSC_VER
typedef unsigned __int64 ull;
#else
typedef unsigned long long ull;
#endif

/* 常量定义 */
enum { BOARD_DIMENSION = 12 };
enum { INVALID_POSITION = -1 };
enum { MAX_MOVES = BOARD_DIMENSION * BOARD_DIMENSION };

static const int OFFSET_X[8] = { -1,-1,-1,0,0,1,1,1 };
static const int OFFSET_Y[8] = { -1,0,1,-1,1,-1,0,1 };

enum {
    MAX_EVALUATION_SCORE = 1000000000,
    MIN_EVALUATION_SCORE = -1000000000,
    FIVE_IN_A_LINE_SCORE = 10000000,
    LIVE_FOUR_SCORE = 100000,
    BLOCKED_FOUR_SCORE = 1000,
    LIVE_THREE_SCORE = 1000,
    BLOCKED_THREE_SCORE = 100,
    LIVE_TWO_SCORE = 100,
    BLOCKED_TWO_SCORE = 10,
    LIVE_ONE_SCORE = 10,
    BLOCKED_ONE_SCORE = 1,
    SEARCH_DEPTH_LEVEL = 3
};

/* 枚举 */
typedef enum { EmptyPosition = 0, BlackPiece = 1, WhitePiece = 2 } PieceStatus;
typedef enum { OPENING_PHASE, MIDGAME_PHASE, ENDGAME_PHASE } GamePhase;

/* 结构体 */
typedef struct {
    int x;
    int y;
} OpeningMove;

typedef struct {
    int x;
    int y;
    int score;
} ChessMove;

typedef struct {
    int searchDepth;
    int positionScore;
} HashTableEntry;

/* 开局库条目（key 存为 C 字符串 "x1,y1,x2,y2,..."） */
typedef struct {
    char* key;            /* 动态分配的字符串 */
    OpeningMove move;
} OpeningEntry;

/* 全局游戏状态（代替 C++ 类成员） */
static PieceStatus gameBoard[BOARD_DIMENSION][BOARD_DIMENSION];
static PieceStatus ourPieceColor = EmptyPosition;
static PieceStatus opponentPieceColor = EmptyPosition;
static int ourPieceScores[BOARD_DIMENSION][BOARD_DIMENSION];
static int opponentPieceScores[BOARD_DIMENSION][BOARD_DIMENSION];
static ull zobrist[BOARD_DIMENSION][BOARD_DIMENSION][3];
static ull currentZobrist = 0ULL;
static int moveCounter = 0;
typedef struct { int x, y; PieceStatus flag; } MoveHistoryRecord;
static MoveHistoryRecord moveHistory[MAX_MOVES];

/* 开局库（简单数组） */
static OpeningEntry* openingBook = NULL;
static int openingBookSize = 0;
static int openingBookCapacity = 0;

/* 置换表：采用开地址线性探测（固定大小） */
#define TRANSPOSITION_TABLE_SIZE (1 << 16) /* 65536 */
static ull tt_keys[TRANSPOSITION_TABLE_SIZE];
static int tt_depth[TRANSPOSITION_TABLE_SIZE];
static int tt_score[TRANSPOSITION_TABLE_SIZE];

//strdup
static char* my_strdup(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* res = (char*)malloc(len);
    if (res != NULL) {
        strcpy(res, s);
    }
    return res;
}
/* 随机数生成（xorshift64*）用于 Zobrist */
static ull rng_state;
static ull rng_next64(void) {
    /* xorshift64* */
    ull x = rng_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    rng_state = x;
    return x * (ull)2685821657736338717ULL;
}

/* 工具：确保开局库容量 */
static void openingBookEnsureCapacity(int need) {
    if (openingBookCapacity >= need) return;
    int newcap = openingBookCapacity ? openingBookCapacity * 2 : 128;
    while (newcap < need) newcap *= 2;
    openingBook = (OpeningEntry*)realloc(openingBook, sizeof(OpeningEntry) * newcap);
    openingBookCapacity = newcap;
}

/* 插入开局库 */
static void openingBookAddRaw(const char* key, int x, int y) {
    openingBookEnsureCapacity(openingBookSize + 1);
    openingBook[openingBookSize].key = my_strdup(key);  // 使用自定义函数
    openingBook[openingBookSize].move.x = x;
    openingBook[openingBookSize].move.y = y;
    openingBookSize++;
}

/* 生成对称 key（symmetry 0..5）与插入 */
static void generate_symmetric_and_add(const char* key, int symmetry, int add_x, int add_y) {
    /* 解析 key into array of ints */
    int coords[256];
    int coords_len = 0;
    char tmp[1024];
    strncpy(tmp, key, sizeof(tmp) - 1); tmp[sizeof(tmp) - 1] = 0;
    char* p = tmp;
    char* tok;
    while ((tok = strtok(p, ",")) != NULL) {
        p = NULL;
        coords[coords_len++] = atoi(tok);
    }
    /* build new key */
    char buf[1024];
    buf[0] = 0;
    for (int i = 0; i + 1 < coords_len; i += 2) {
        int x = coords[i], y = coords[i + 1];
        int nx = x, ny = y;
        switch (symmetry) {
        case 1: nx = BOARD_DIMENSION - 1 - x; break; /* horizontal */
        case 2: ny = BOARD_DIMENSION - 1 - y; break; /* vertical */
        case 3: { int t = nx; nx = ny; ny = t; } break; /* main diagonal swap */
        case 4: nx = BOARD_DIMENSION - 1 - y; ny = BOARD_DIMENSION - 1 - x; break; /* anti-diag */
        case 5: nx = BOARD_DIMENSION - 1 - x; ny = BOARD_DIMENSION - 1 - y; break; /* rot180 */
        default: break;
        }
        char tbuf[32];
        if (i > 0) strcat(buf, ",");
        sprintf(tbuf, "%d,%d", nx, ny);
        strcat(buf, tbuf);
    }
    openingBookAddRaw(buf, add_x, add_y);
}

/* 添加开局，并自动添加多种对称形式 */
static void addOpeningWithSymmetries(const char* key, int movex, int movey) {
    /* 原型 */
    openingBookAddRaw(key, movex, movey);
    /* 五种对称 */
    for (int t = 1; t <= 5; ++t) {
        generate_symmetric_and_add(key, t, movex, movey);
    }
}

/* 初始化开局库（将原 C++ 中 list 的内容逐项加入） */
static void initOpeningBook(void) {
    /* 先释放旧数据（如果有） */
    for (int i = 0; i < openingBookSize; ++i) {
        free(openingBook[i].key);
    }
    free(openingBook);
    openingBook = NULL;
    openingBookSize = 0;
    openingBookCapacity = 0;

    /* 添加条目（尽量复刻原 cpp 的条目） */
    addOpeningWithSymmetries("5,5,6,5,7,5", 8, 5);
    addOpeningWithSymmetries("5,5,6,5,7,6", 8, 6);
    addOpeningWithSymmetries("5,5,6,5,7,4", 8, 4);
    addOpeningWithSymmetries("5,5,6,5,6,6", 7, 7);
    addOpeningWithSymmetries("5,5,6,5,6,4", 7, 3);
    addOpeningWithSymmetries("5,5,6,5,5,6", 4, 7);
    addOpeningWithSymmetries("5,5,6,5,7,7", 4, 6);
    addOpeningWithSymmetries("5,5,6,5,5,4", 4, 3);
    addOpeningWithSymmetries("5,5,6,5,4,6", 3, 7);
    addOpeningWithSymmetries("5,5,6,5,4,4", 3, 3);
    addOpeningWithSymmetries("5,5,6,5,4,5", 3, 5);
    addOpeningWithSymmetries("5,5,6,5,5,3", 4, 2);
    addOpeningWithSymmetries("5,5,6,5,4,7", 2, 7);

    /* 斜指类等（摘录原 cpp） */
    addOpeningWithSymmetries("5,5,6,6,7,7", 4, 6);
    addOpeningWithSymmetries("5,5,6,6,7,6", 8, 5);
    addOpeningWithSymmetries("5,5,6,6,7,5", 7, 8);
    addOpeningWithSymmetries("5,5,6,6,7,4", 8, 4);
    addOpeningWithSymmetries("5,5,6,6,8,4", 8, 5);
    addOpeningWithSymmetries("5,5,6,6,6,7", 7, 8);
    addOpeningWithSymmetries("5,5,6,6,6,7", 5, 8);
    addOpeningWithSymmetries("5,5,6,6,5,7", 4, 8);
    addOpeningWithSymmetries("5,5,6,6,7,5", 8, 7);
    addOpeningWithSymmetries("5,5,6,6,5,6", 4, 7);
    addOpeningWithSymmetries("5,5,6,6,4,7", 3, 8);
    addOpeningWithSymmetries("5,5,6,6,4,6", 3, 7);
    addOpeningWithSymmetries("5,5,6,6,4,4", 3, 3);

    /* 扩展 AI 自定义开局库（原文） */
    addOpeningWithSymmetries("5,5,6,6,6,5,5,6", 7, 7);
    addOpeningWithSymmetries("6,6,7,7,7,6,6,7", 8, 8);
    addOpeningWithSymmetries("5,5,6,6,5,6,6,7", 7, 7);
    addOpeningWithSymmetries("5,5,6,5,7,5,8,5", 9, 5);
    addOpeningWithSymmetries("5,5,5,6,5,7,5,8", 5, 9);
    addOpeningWithSymmetries("6,6,6,7,5,6,7,7", 8, 6);
    addOpeningWithSymmetries("5,5,5,6,6,6,6,7", 7, 6);
    addOpeningWithSymmetries("5,5,4,4,3,3", 5, 4);
    addOpeningWithSymmetries("5,5,7,7,8,8", 6, 6);
    addOpeningWithSymmetries("6,6,8,8,7,7", 7, 6);
}

/* 置换表操作（线性探测） */
static void tt_clear(void) {
    for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i) {
        tt_keys[i] = 0ULL;
        tt_depth[i] = INT_MIN;
        tt_score[i] = 0;
    }
}
static int tt_index_for_key(ull key) {
    /* 使用简单掩码 */
    return (int)(key & (TRANSPOSITION_TABLE_SIZE - 1));
}
static int tt_get(ull key, int depth, int* out_score) {
    int idx = tt_index_for_key(key);
    for (int probe = 0; probe < TRANSPOSITION_TABLE_SIZE; ++probe) {
        int i = (idx + probe) & (TRANSPOSITION_TABLE_SIZE - 1);
        if (tt_keys[i] == 0ULL) return 0; /* not found */
        if (tt_keys[i] == key) {
            if (tt_depth[i] >= depth) {
                *out_score = tt_score[i];
                return 1;
            }
            else return 0;
        }
    }
    return 0;
}
static void tt_put(ull key, int depth, int score) {
    int idx = tt_index_for_key(key);
    for (int probe = 0; probe < TRANSPOSITION_TABLE_SIZE; ++probe) {
        int i = (idx + probe) & (TRANSPOSITION_TABLE_SIZE - 1);
        if (tt_keys[i] == 0ULL || tt_keys[i] == key || tt_depth[i] < depth) {
            tt_keys[i] = key;
            tt_depth[i] = depth;
            tt_score[i] = score;
            return;
        }
    }
}

/* 辅助：检查坐标合法 */
static inline int isWithinBoardBounds(int x, int y) {
    return (x >= 0 && x < BOARD_DIMENSION && y >= 0 && y < BOARD_DIMENSION);
}

/* 更新 Zobrist 哈希（切换位置 old->new） */
static inline void updateZobrist(int x, int y, PieceStatus oldp, PieceStatus newp) {
    currentZobrist ^= zobrist[x][y][oldp];
    currentZobrist ^= zobrist[x][y][newp];
}

/* 棋型评估辅助（对应 C++ evaluatePositionState） */
static int evaluatePositionState(const int pieceCount, const int blockCount, const int emptyCount) {
    if (emptyCount <= 0) {
        if (pieceCount >= 5) return FIVE_IN_A_LINE_SCORE;
        if (blockCount == 0) {
            switch (pieceCount) {
            case 1: return LIVE_ONE_SCORE;
            case 2: return LIVE_TWO_SCORE;
            case 3: return LIVE_THREE_SCORE;
            case 4: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 1: return BLOCKED_ONE_SCORE;
            case 2: return BLOCKED_TWO_SCORE;
            case 3: return BLOCKED_THREE_SCORE;
            case 4: return BLOCKED_FOUR_SCORE;
            }
        }
    }
    else if (emptyCount == 1 || emptyCount == pieceCount - 1) {
        if (pieceCount >= 6) return FIVE_IN_A_LINE_SCORE;
        if (blockCount == 0) {
            switch (pieceCount) {
            case 2: return LIVE_TWO_SCORE;
            case 3: return LIVE_THREE_SCORE;
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 2: return BLOCKED_TWO_SCORE;
            case 3: return BLOCKED_THREE_SCORE;
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            }
        }
    }
    else if (emptyCount == 2 || emptyCount == pieceCount - 2) {
        if (pieceCount >= 7) return FIVE_IN_A_LINE_SCORE;
        if (blockCount == 0) {
            switch (pieceCount) {
            case 3: return LIVE_THREE_SCORE;
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            case 6: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 3: return BLOCKED_THREE_SCORE;
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            case 6: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            case 6: return BLOCKED_FOUR_SCORE;
            }
        }
    }
    else if (emptyCount == 3 || emptyCount == pieceCount - 3) {
        if (pieceCount >= 8) return FIVE_IN_A_LINE_SCORE;
        if (blockCount == 0) {
            switch (pieceCount) {
            case 4: return LIVE_THREE_SCORE;
            case 5: return LIVE_THREE_SCORE;
            case 6: return BLOCKED_FOUR_SCORE;
            case 7: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            case 6: return BLOCKED_FOUR_SCORE;
            case 7: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 4: return BLOCKED_FOUR_SCORE;
            case 5: return BLOCKED_FOUR_SCORE;
            case 6: return BLOCKED_FOUR_SCORE;
            case 7: return BLOCKED_FOUR_SCORE;
            }
        }
    }
    else if (emptyCount == 4 || emptyCount == pieceCount - 4) {
        if (pieceCount >= 9) return FIVE_IN_A_LINE_SCORE;
        if (blockCount == 0) {
            switch (pieceCount) {
            case 5: case 6: case 7: case 8: return LIVE_FOUR_SCORE;
            }
        }
        if (blockCount == 1) {
            switch (pieceCount) {
            case 4: case 5: case 6: case 7: case 8: return BLOCKED_FOUR_SCORE;
            }
        }
        if (blockCount == 2) {
            switch (pieceCount) {
            case 5: case 6: case 7: case 8: return BLOCKED_FOUR_SCORE;
            }
        }
    }
    else if (emptyCount == 5 || emptyCount == pieceCount - 5) {
        return FIVE_IN_A_LINE_SCORE;
    }
    return 0;
}

/* evaluateSinglePoint —— 评估某一点（与 C++ 版本对应） */
static int evaluateSinglePoint(int x, int y, PieceStatus pieceFlag) {
    int totalScore = 0;
    int consecutiveCount, blockCount, emptyPositionCount;
    /* 横向 */
    consecutiveCount = 1; blockCount = 0; emptyPositionCount = -1;
    /* 向右 */
    for (int i = y + 1; ; ++i) {
        if (i >= BOARD_DIMENSION) { blockCount++; break; }
        if (gameBoard[x][i] == EmptyPosition) {
            if (emptyPositionCount == -1 && i < BOARD_DIMENSION - 1 && gameBoard[x][i + 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount; continue;
            }
            else break;
        }
        else if (gameBoard[x][i] == pieceFlag) {
            consecutiveCount++; continue;
        }
        else { blockCount++; break; }
    }
    /* 向左 */
    for (int i = y - 1; ; --i) {
        if (i < 0) { blockCount++; break; }
        if (gameBoard[x][i] == EmptyPosition) {
            if (emptyPositionCount == -1 && i > 0 && gameBoard[x][i - 1] == pieceFlag) {
                emptyPositionCount = 0; continue;
            }
            else break;
        }
        else if (gameBoard[x][i] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) emptyPositionCount++;
            continue;
        }
        else { blockCount++; break; }
    }
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 纵向 */
    consecutiveCount = 1; blockCount = 0; emptyPositionCount = -1;
    for (int i = x + 1; ; ++i) {
        if (i >= BOARD_DIMENSION) { blockCount++; break; }
        if (gameBoard[i][y] == EmptyPosition) {
            if (emptyPositionCount == -1 && i < BOARD_DIMENSION - 1 && gameBoard[i + 1][y] == pieceFlag) {
                emptyPositionCount = consecutiveCount; continue;
            }
            else break;
        }
        else if (gameBoard[i][y] == pieceFlag) {
            consecutiveCount++; continue;
        }
        else { blockCount++; break; }
    }
    for (int i = x - 1; ; --i) {
        if (i < 0) { blockCount++; break; }
        if (gameBoard[i][y] == EmptyPosition) {
            if (emptyPositionCount == -1 && i > 0 && gameBoard[i - 1][y] == pieceFlag) {
                emptyPositionCount = 0; continue;
            }
            else break;
        }
        else if (gameBoard[i][y] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) emptyPositionCount++;
            continue;
        }
        else { blockCount++; break; }
    }
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 主对角线 \ */
    consecutiveCount = 1; blockCount = 0; emptyPositionCount = -1;
    for (int i = 1; ; ++i) {
        int cx = x + i, cy = y + i;
        if (cx >= BOARD_DIMENSION || cy >= BOARD_DIMENSION) { blockCount++; break; }
        if (gameBoard[cx][cy] == EmptyPosition) {
            if (emptyPositionCount == -1 && (cx < BOARD_DIMENSION - 1 && cy < BOARD_DIMENSION - 1) && gameBoard[cx + 1][cy + 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount; continue;
            }
            else break;
        }
        else if (gameBoard[cx][cy] == pieceFlag) {
            consecutiveCount++; continue;
        }
        else { blockCount++; break; }
    }
    for (int i = 1; ; ++i) {
        int cx = x - i, cy = y - i;
        if (cx < 0 || cy < 0) { blockCount++; break; }
        if (gameBoard[cx][cy] == EmptyPosition) {
            if (emptyPositionCount == -1 && (cx > 0 && cy > 0) && gameBoard[cx - 1][cy - 1] == pieceFlag) {
                emptyPositionCount = 0; continue;
            }
            else break;
        }
        else if (gameBoard[cx][cy] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount != -1) emptyPositionCount++;
            continue;
        }
        else { blockCount++; break; }
    }
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    /* 副对角线 / */
    consecutiveCount = 1; blockCount = 0; emptyPositionCount = -1;
    for (int i = 1; ; ++i) {
        int cx = x + i, cy = y - i;
        if (cx < 0 || cy < 0 || cx >= BOARD_DIMENSION || cy >= BOARD_DIMENSION) { blockCount++; break; }
        if (gameBoard[cx][cy] == EmptyPosition) {
            if (emptyPositionCount == -1 && (cx < BOARD_DIMENSION - 1 && cy > 0) && gameBoard[cx + 1][cy - 1] == pieceFlag) {
                emptyPositionCount = consecutiveCount; continue;
            }
            else break;
        }
        else if (gameBoard[cx][cy] == pieceFlag) {
            consecutiveCount++; continue;
        }
        else { blockCount++; break; }
    }
    for (int i = 1; ; ++i) {
        int cx = x - i, cy = y + i;
        if (cx < 0 || cy < 0 || cx >= BOARD_DIMENSION || cy >= BOARD_DIMENSION) { blockCount++; break; }
        if (gameBoard[cx][cy] == EmptyPosition) {
            if (emptyPositionCount == -1 && (cx > 0 && cy < BOARD_DIMENSION - 1) && gameBoard[cx - 1][cy + 1] == pieceFlag) {
                emptyPositionCount = 0; continue;
            }
            else break;
        }
        else if (gameBoard[cx][cy] == pieceFlag) {
            consecutiveCount++;
            if (emptyPositionCount == -1) emptyPositionCount++;
            continue;
        }
        else { blockCount++; break; }
    }
    totalScore += evaluatePositionState(consecutiveCount, blockCount, emptyPositionCount);

    return totalScore;
}

/* updatePositionScores */
static void updatePositionScores(int x, int y) {
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
    for (int dir = 0; dir < 8; ++dir) {
        int nx = x, ny = y;
        while (1) {
            nx += OFFSET_X[dir];
            ny += OFFSET_Y[dir];
            if (!isWithinBoardBounds(nx, ny)) break;
            if (gameBoard[nx][ny] == ourPieceColor) {
                ourPieceScores[nx][ny] = evaluateSinglePoint(nx, ny, ourPieceColor);
            }
            else if (gameBoard[nx][ny] == opponentPieceColor) {
                opponentPieceScores[nx][ny] = evaluateSinglePoint(nx, ny, opponentPieceColor);
            }
        }
    }
}

/* makeChessMove: 落子（pieceFlag==EmptyPosition 表示撤子） */
static void makeChessMove(int x, int y, PieceStatus pieceFlag) {
    /* 更新 zobrist */
    updateZobrist(x, y, gameBoard[x][y], pieceFlag);
    gameBoard[x][y] = pieceFlag;
    updateZobrist(x, y, EmptyPosition, EmptyPosition); /* no-op to keep same pattern (safe) */
    updatePositionScores(x, y);
    if (pieceFlag != EmptyPosition) {
        if (moveCounter < MAX_MOVES) {
            moveHistory[moveCounter].x = x;
            moveHistory[moveCounter].y = y;
            moveHistory[moveCounter].flag = pieceFlag;
            moveCounter++;
        }
    }
    else {
        /* 撤子：在本实现中，撤子操作不修改 moveHistory 的记录（negamax 用: makeChessMove(i,j) 撤销）
           为了撤销，我们需要将对应位置重置为空并 moveCounter--（只在 negamax 的撤销路径使用）
        */
        /* 实际上上层代码调用撤销时会管理 moveCounter，我们提供简单操作（上层保证正确调用顺序） */
    }
}

/* findEmptyPositionToPlace：找到第一个空位作为后备落子 */
static void findEmptyPositionToPlace(PieceStatus pf, int* bestX, int* bestY) {
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            if (gameBoard[i][j] == EmptyPosition) {
                *bestX = i; *bestY = j; return;
            }
}

/* checkForFiveInARow */
static int checkForFiveInARow(int x, int y, PieceStatus pf) {
    int consecutive;
    for (int dir = 4; dir < 8; ++dir) {
        consecutive = 1;
        int i = x + OFFSET_X[dir], j = y + OFFSET_Y[dir];
        while (isWithinBoardBounds(i, j) && gameBoard[i][j] == pf) {
            consecutive++; i += OFFSET_X[dir]; j += OFFSET_Y[dir];
        }
        i = x - OFFSET_X[dir]; j = y - OFFSET_Y[dir];
        while (isWithinBoardBounds(i, j) && gameBoard[i][j] == pf) {
            consecutive++; i -= OFFSET_X[dir]; j -= OFFSET_Y[dir];
        }
        if (consecutive >= 5) return 1;
    }
    return 0;
}

/* checkLinePattern (对应 C++ 的 checkLinePattern) */
static int checkLinePattern(int x, int y, PieceStatus pf, int targetCount, int targetEmpty) {
    for (int dir = 4; dir < 8; ++dir) {
        int consecutive = 1, emptyCount = 0;
        int i = x + OFFSET_X[dir], j = y + OFFSET_Y[dir];
        while (isWithinBoardBounds(i, j)) {
            if (gameBoard[i][j] == pf) { consecutive++; }
            else if (gameBoard[i][j] == EmptyPosition) { emptyCount++; break; }
            else break;
            i += OFFSET_X[dir]; j += OFFSET_Y[dir];
        }
        i = x - OFFSET_X[dir]; j = y - OFFSET_Y[dir];
        while (isWithinBoardBounds(i, j)) {
            if (gameBoard[i][j] == pf) { consecutive++; }
            else if (gameBoard[i][j] == EmptyPosition) { emptyCount++; break; }
            else break;
            i -= OFFSET_X[dir]; j -= OFFSET_Y[dir];
        }
        if (consecutive == targetCount && emptyCount == targetEmpty) return 1;
    }
    return 0;
}

/* generateCandidateMoves: 填充 moves 数组并返回 movesLen（最多保留前 6） */
static int cmpChessMoveDesc(const void* a, const void* b) {
    const ChessMove* A = (const ChessMove*)a;
    const ChessMove* B = (const ChessMove*)b;
    return (B->score - A->score); /* 降序 */
}
static void generateCandidateMoves(ChessMove* moves, int* movesLen, PieceStatus pf) {
    int len = 0;
    for (int i = 0; i < BOARD_DIMENSION; ++i) {
        for (int j = 0; j < BOARD_DIMENSION; ++j) {
            if (gameBoard[i][j] == EmptyPosition) {
                /* 只考虑周围有子的位置 */
                int found = 0;
                for (int dx = -1; dx <= 1 && !found; ++dx)
                    for (int dy = -1; dy <= 1 && !found; ++dy) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = i + dx, ny = j + dy;
                        if (isWithinBoardBounds(nx, ny) && gameBoard[nx][ny] != EmptyPosition) found = 1;
                    }
                if (!found) continue;
                moves[len].x = i; moves[len].y = j;
                moves[len].score = evaluateSinglePoint(i, j, pf);
                len++;
            }
        }
    }
    if (len > 1) qsort(moves, len, sizeof(ChessMove), cmpChessMoveDesc);
    if (len > 6) len = 6;
    *movesLen = len;
}

/* evaluateOverallBoard */
static int evaluateOverallBoard(void) {
    int val = 0;
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j) {
            if (gameBoard[i][j] == ourPieceColor) val += ourPieceScores[i][j];
            else if (gameBoard[i][j] == opponentPieceColor) val -= opponentPieceScores[i][j];
        }
    return val;
}

/* negamax with alpha-beta pruning
   注意：为了简化撤子，我们手动做落子/撤子并维护 moveCounter
*/
static int negamaxWithAlphaBetaPruning(int depth, int alpha, int beta, PieceStatus pf, int* bestX, int* bestY, int isTopLevel) {
    int stored;
    if (tt_get(currentZobrist, depth, &stored)) {
        return stored;
    }
    if (depth == 0) {
        int score = -evaluateOverallBoard();
        tt_put(currentZobrist, depth, score);
        return score;
    }
    ChessMove moves[MAX_MOVES];
    int movesLen = 0;
    generateCandidateMoves(moves, &movesLen, pf);

    int bestLocalX = INVALID_POSITION, bestLocalY = INVALID_POSITION;
    if (movesLen == 0) {
        /* no candidate moves -> fallback */
        int sx = INVALID_POSITION, sy = INVALID_POSITION;
        findEmptyPositionToPlace(pf, &sx, &sy);
        if (sx == INVALID_POSITION) return 0;
        /* 应该尝试这一走，递归 */
        /* 落子 */
        PieceStatus old = gameBoard[sx][sy];
        updateZobrist(sx, sy, old, pf);
        gameBoard[sx][sy] = pf;
        updatePositionScores(sx, sy);
        moveHistory[moveCounter].x = sx; moveHistory[moveCounter].y = sy; moveHistory[moveCounter].flag = pf; moveCounter++;

        int val = -negamaxWithAlphaBetaPruning(depth - 1, -beta, -alpha, (pf == BlackPiece) ? WhitePiece : BlackPiece, bestX, bestY, 0);

        /* 撤子 */
        moveCounter--;
        gameBoard[sx][sy] = old;
        updateZobrist(sx, sy, pf, old);
        updatePositionScores(sx, sy);

        if (val >= beta) {
            tt_put(currentZobrist, depth, beta);
            return beta;
        }
        if (val > alpha) {
            alpha = val;
            bestLocalX = sx; bestLocalY = sy;
        }
        if (isTopLevel && bestLocalX != INVALID_POSITION) { *bestX = bestLocalX; *bestY = bestLocalY; }
        tt_put(currentZobrist, depth, alpha);
        return alpha;
    }

    for (int k = 0; k < movesLen; ++k) {
        int i = moves[k].x, j = moves[k].y;
        /* 落子 */
        PieceStatus old = gameBoard[i][j];
        updateZobrist(i, j, old, pf);
        gameBoard[i][j] = pf;
        updatePositionScores(i, j);
        moveHistory[moveCounter].x = i; moveHistory[moveCounter].y = j; moveHistory[moveCounter].flag = pf; moveCounter++;

        int val = -negamaxWithAlphaBetaPruning(depth - 1, -beta, -alpha, (pf == BlackPiece) ? WhitePiece : BlackPiece, bestX, bestY, 0);

        /* 撤子 */
        moveCounter--;
        gameBoard[i][j] = old;
        updateZobrist(i, j, pf, old);
        updatePositionScores(i, j);

        if (val >= beta) {
            tt_put(currentZobrist, depth, beta);
            return beta;
        }
        if (val > alpha) {
            alpha = val;
            bestLocalX = i; bestLocalY = j;
        }
    }
    if (isTopLevel && bestLocalX != INVALID_POSITION) {
        *bestX = bestLocalX; *bestY = bestLocalY;
    }
    tt_put(currentZobrist, depth, alpha);
    return alpha;
}

/* attemptNextMove：检查开局库、特定步数优先走法、双方直接或威胁性走法 */
static int attemptNextMove(int* nextX, int* nextY, int* score) {
    /* 生成当前历史序列 key */
    char seq[4096]; seq[0] = 0;
    for (int step = 0; step < moveCounter; ++step) {
        char t[32];
        if (step > 0) strcat(seq, ",");
        sprintf(t, "%d,%d", moveHistory[step].x, moveHistory[step].y);
        strcat(seq, t);
    }
    /* 查找开局库 */
    for (int i = 0; i < openingBookSize; ++i) {
        if (strcmp(openingBook[i].key, seq) == 0) {
            *nextX = openingBook[i].move.x;
            *nextY = openingBook[i].move.y;
            return 0; /* use opening book, no further search */
        }
    }
    /* 特定步数默认走法（与原 cpp 保持一致） */
    if (moveCounter == 4) {
        *nextX = 4; *nextY = 4; return 0;
    }
    if (moveCounter == 5) {
        if (gameBoard[4][7] == BlackPiece) { *nextX = 7; *nextY = 4; return 0; }
        if (gameBoard[7][4] == BlackPiece) { *nextX = 4; *nextY = 7; return 0; }
        if (gameBoard[4][4] == BlackPiece) { *nextX = 5; *nextY = 7; return 0; }
        if (gameBoard[7][7] == BlackPiece) { *nextX = 6; *nextY = 4; return 0; }
    }
    /* 检查我方直接获胜（五连） */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            if (gameBoard[i][j] == EmptyPosition && checkForFiveInARow(i, j, ourPieceColor)) {
                *nextX = i; *nextY = j; return 0;
            }
    /* 检查对手直接获胜（需阻止） */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            if (gameBoard[i][j] == EmptyPosition && checkForFiveInARow(i, j, opponentPieceColor)) {
                *nextX = i; *nextY = j; return 0;
            }
    /* 检查我方优势（活四 或 冲四+活三） */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            if (gameBoard[i][j] == EmptyPosition &&
                (checkLinePattern(i, j, ourPieceColor, 4, 2) ||
                    (checkLinePattern(i, j, ourPieceColor, 4, 1) && checkLinePattern(i, j, ourPieceColor, 3, 2)))) {
                *nextX = i; *nextY = j; return 0;
            }
    /* 检查对手优势 */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            if (gameBoard[i][j] == EmptyPosition &&
                (checkLinePattern(i, j, opponentPieceColor, 4, 2) ||
                    (checkLinePattern(i, j, opponentPieceColor, 4, 1) && checkLinePattern(i, j, opponentPieceColor, 3, 2)))) {
                *nextX = i; *nextY = j; return 0;
            }
    return 1; /* 需要进一步搜索 */
}

/* 初始化 Zobrist 表、棋盘、开局库、置换表 */
static void initGame(void) {
    /* 初始化 RNG */
    rng_state = (ull)time(NULL) | 1ULL;
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            for (int k = 0; k < 3; ++k)
                zobrist[i][j][k] = rng_next64();

    currentZobrist = 0ULL;
    /* 清空棋盘 */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j) {
            gameBoard[i][j] = EmptyPosition;
            ourPieceScores[i][j] = opponentPieceScores[i][j] = 0;
        }
    moveCounter = 0;
    initOpeningBook();
    tt_clear();
}

/* 处理 START 命令 */
static void handleStart(void) {
    int color;
    if (scanf("%d", &color) != 1) return;
    ourPieceColor = (PieceStatus)color;
    opponentPieceColor = (ourPieceColor == BlackPiece) ? WhitePiece : BlackPiece;
    /* 初始化 Zobrist、开局库等 */
    initGame();
    /* 原 cpp 在 START 里下了前 4 步默认棋（5,5 白; 6,6 白; 5,6 黑; 6,5 黑） */
    /* 在 C++ 版本它调用 makeChessMove(5,5,WhitePiece) 等，我们在这里复刻 */
    makeChessMove(5, 5, WhitePiece);
    makeChessMove(6, 6, WhitePiece);
    makeChessMove(5, 6, BlackPiece);
    makeChessMove(6, 5, BlackPiece);
    /* moveCounter 在 makeChessMove 中已递增 */

    printf("OK\n");
    fflush(stdout);
}

/* 处理 PLACE x y（对手下子） */
static void handlePlace(int x, int y) {
    makeChessMove(x, y, opponentPieceColor);
    /* moveCounter 已在 makeChessMove 中递增 */
}

/* 处理 TURN（轮到我下） */
static void handleTurn(void) {
    int nextX = INVALID_POSITION, nextY = INVALID_POSITION;
    int score = 0;
    int needSearch = attemptNextMove(&nextX, &nextY, &score);
    if (needSearch) {
        /* 使用 negamax 搜索 */
        int bestX = INVALID_POSITION, bestY = INVALID_POSITION;
        /* 调用时传入 ourPieceColor */
        negamaxWithAlphaBetaPruning(SEARCH_DEPTH_LEVEL, MIN_EVALUATION_SCORE, MAX_EVALUATION_SCORE, ourPieceColor, &bestX, &bestY, 1);
        if (bestX != INVALID_POSITION) {
            nextX = bestX; nextY = bestY;
        }
    }
    if (nextX == INVALID_POSITION || nextY == INVALID_POSITION) {
        findEmptyPositionToPlace(ourPieceColor, &nextX, &nextY);
    }
    makeChessMove(nextX, nextY, ourPieceColor);
    printf("%d %d\n", nextX, nextY);
    fflush(stdout);
}

/* 处理 END */
static void handleEnd(int result) {
    fprintf(stderr, "Game ended. Result=%d\n", result);
    fflush(stderr);
}

/* 主循环 run */
static void run_loop(void) {
    char cmd[64];
    while (1) {
        if (scanf("%63s", cmd) == EOF) break;
        if (strcmp(cmd, "START") == 0) {
            handleStart();
        }
        else if (strcmp(cmd, "PLACE") == 0) {
            int x, y;
            if (scanf("%d %d", &x, &y) == 2) {
                handlePlace(x, y);
            }
        }
        else if (strcmp(cmd, "TURN") == 0) {
            handleTurn();
        }
        else if (strcmp(cmd, "END") == 0) {
            int res;
            if (scanf("%d", &res) == 1) handleEnd(res);
            break;
        }
        else {
            fprintf(stderr, "Unknown command: %s\n", cmd);
            fflush(stderr);
        }
    }
}

/* main */
int main(void) {
    /* 初始种子和基本初始化（zobrist 会在 START 里再次初始化） */
    rng_state = (ull)time(NULL) | 1ULL;
    /* 清零棋盘以防万一 */
    for (int i = 0; i < BOARD_DIMENSION; ++i)
        for (int j = 0; j < BOARD_DIMENSION; ++j)
            gameBoard[i][j] = EmptyPosition;
    /* 初始化置换表和开局库 */
    initOpeningBook(); /* 先初始化开局库（里面依赖 RNG 会用到 rng_next64 若未初始化则也可运行） */
    tt_clear();

    run_loop();

    /* 释放开局库内存 */
    for (int i = 0; i < openingBookSize; ++i) free(openingBook[i].key);
    free(openingBook);
    openingBook = NULL;

    return 0;
}
