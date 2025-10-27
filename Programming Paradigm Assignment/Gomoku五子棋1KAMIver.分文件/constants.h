#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

// ���Ͷ���
typedef long long LLS;
typedef unsigned long long ULL;

// ������س���
#define GRID_DIM 12
#define BLANK 0
#define PIECE_B 1
#define PIECE_W 2

// ���ֳ���
#define FIVE5 7
#define LIVE4 6
#define RUSH4 5
#define LIVE3 4
#define SLEEPING3 3
#define LIVE2 2
#define SLEEPING2 1
#define JUMPLIVE3 8
#define JUMPSLEEPING4 9
#define JUMPLIVE4 10
#define STYLENUM 11

// ������س���
#define MAX_DEPTH 7
#define TIME_LIMIT 1900
#define MAX_MOVES 121

// ��ϣ��س���
#define HASH_SIZE (1 << 20)
#define HASH_EXACT 0
#define HASH_ALPHA 1
#define HASH_BETA 2

// �����
#define CMD_INIT "START"
#define CMD_SET "PLACE"
#define CMD_PLAY "TURN"
#define CMD_FINISH "END"
#define CMD_CRACKER "CRACK"
#define CMD_ERASE "ERASE"
#define CMD_LIMIT_CHECK "LIMIT"
#define CMD_SHOW_BOARD "SHOW"
#define CMD_BUGMODE "LIBERTY_STEP"

#endif // CONSTANTS_H