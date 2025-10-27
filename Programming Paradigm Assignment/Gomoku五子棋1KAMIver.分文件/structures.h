#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "constants.h"

// 哈希条目结构
struct HashEntry {
    ULL key;
    int depth;
    LLS score;
    int type;
};

// 棋形结构
struct chessshape {
    int score;
    char shape[9];
};

// 棋型结构
struct ChessStyle {
    LLS score;
    ChessStyle(int s = 0) : score(s) {}
};

// 所有棋型结构
struct AllChessStyle {
    ChessStyle MyStyle[STYLENUM];
    ChessStyle EnemyStyle[STYLENUM];
    AllChessStyle();
};

// 位置结构
struct Position {
    int row = -1;
    int col = -1;
    LLS score = 0;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
};

// 移动列表结构
struct MoveList {
    Position moves[MAX_MOVES];
    int count = 0;
};

// 游戏状态结构
struct GameState {
    int grid[GRID_DIM][GRID_DIM] = { { 0 } };
    int gamePhase = 0;
    ULL hash;

    GameState();
    void initializeBoard();
    void updateGrid(int row, int col, int piece);
};

#endif // STRUCTURES_H