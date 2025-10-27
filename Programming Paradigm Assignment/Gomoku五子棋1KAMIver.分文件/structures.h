#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "constants.h"

// ��ϣ��Ŀ�ṹ
struct HashEntry {
    ULL key;
    int depth;
    LLS score;
    int type;
};

// ���νṹ
struct chessshape {
    int score;
    char shape[9];
};

// ���ͽṹ
struct ChessStyle {
    LLS score;
    ChessStyle(int s = 0) : score(s) {}
};

// �������ͽṹ
struct AllChessStyle {
    ChessStyle MyStyle[STYLENUM];
    ChessStyle EnemyStyle[STYLENUM];
    AllChessStyle();
};

// λ�ýṹ
struct Position {
    int row = -1;
    int col = -1;
    LLS score = 0;
    Position(int r = -1, int c = -1) : row(r), col(c) {}
};

// �ƶ��б�ṹ
struct MoveList {
    Position moves[MAX_MOVES];
    int count = 0;
};

// ��Ϸ״̬�ṹ
struct GameState {
    int grid[GRID_DIM][GRID_DIM] = { { 0 } };
    int gamePhase = 0;
    ULL hash;

    GameState();
    void initializeBoard();
    void updateGrid(int row, int col, int piece);
};

#endif // STRUCTURES_H