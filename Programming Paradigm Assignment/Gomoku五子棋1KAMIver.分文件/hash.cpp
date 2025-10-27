#include "constants.h"
#include "structures.h"
#include <cstdlib>
#include <cstring>
#include <ctime>

// 全局变量
ULL zobristTable[3][GRID_DIM][GRID_DIM];
HashEntry* hashTable;

// 生成64位随机数
ULL random64() {
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

// 初始化Zobrist哈希表
void initZobrist() {
    srand(size_t(time(NULL)));
    for (int piece = 0; piece < 3; piece++) {
        for (int i = 0; i < GRID_DIM; i++) {
            for (int j = 0; j < GRID_DIM; j++) {
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

// 查找哈希
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

// 存储哈希
void storeHash(ULL hash, int depth, LLS score, int type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}