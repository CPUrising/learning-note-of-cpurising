#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>

// ���峤���ͱ��������ڴ洢����ֵ����
typedef long long LLS;
using namespace std;

// -------------------------- ���������ӳ������� --------------------------
#define GRID_DIM 12          // ����ά�ȣ�12x12����
#define BLANK 0              // �հ�λ�ñ�ʶ�������ӣ�
#define PIECE_B 1            // �����ʶ
#define PIECE_W 2            // �����ʶ
const LLS MAX_VAL = 2000000000000000000LL;  // �������ֵ������Alpha-Beta��֦��
const LLS MIN_VAL = -2000000000000000000LL; // ������Сֵ������Alpha-Beta��֦��

// -------------------------- ָ������� --------------------------
#define CMD_INIT "START"       // ��ʼ��ָ���֪AIִ����ɫ��
#define CMD_SET "PLACE"        // ��������ָ���֪�����������꣩
#define CMD_PLAY "TURN"        // AI����ָ�Ҫ��AI����������꣩
#define CMD_FINISH "END"       // ��Ϸ����ָ���֪��ʤ����
#define CMD_CRACKER "CRACK"    // ����ָ��ֶ���AI���ӣ�
#define CMD_ERASE "ERASE"      // ����ָ�����ָ��λ�����ӣ�
#define CMD_LIMIT_CHECK "LIMIT"// ����ָ����ʱ�������Ƿ�Ϲ棩
#define CMD_SHOW_BOARD "SHOW"  // ����ָ���ʾ��ǰ����״̬��
#define CMD_BUGMODE "LIBERTY_STEP" // ����ģʽ����ָ��

// -------------------------- ȫ�ֱ��� --------------------------
bool bugmode = false;         // ����ģʽ���أ�Ĭ�Ϲرգ�
clock_t start_turn;           // ��¼�������ӿ�ʼʱ�䣨����ʱ�����Ƽ�飩
int turnround = 0;            // ��ǰ�غ�����δʵ��ʹ�ã�Ԥ����չ��
int myColor;                  // AI�����������ɫ��1=�ڣ�2=�ף�
int rivalColor;               // ���ֵ�������ɫ����AI��ɫ�෴��

// ��������λ�ü��㣨12x12��������Ϊ(5,5)��(5,6)��(6,5)��(6,6)��
const int CENTER_POS_1 = (GRID_DIM + 1) / 2 - 1; // ������/��1��5��
const int CENTER_POS_2 = GRID_DIM / 2;           // ������/��2��6��
// �ĸ���Ҫ�����������¡��ҡ����¡����ϣ������ڼ������
const int dx[] = { 1, 0, 1, 1 };  // �з���ƫ��
const int dy[] = { 0, 1, 1, -1 }; // �з���ƫ��

// -------------------------- Zobrist��ϣ��ض��� --------------------------
#define HASH_SIZE (1 << 20)    // ��ϣ���С��2^20 = 1,048,576��ƽ��ռ����ͻ��
#define HASH_EXACT 0           // ��ȷ��ϣ����ǰ����µ�׼ȷ���֣�
#define HASH_ALPHA 1           // Alpha��ϣ����ǰ���֡�Alpha��������̽����
#define HASH_BETA 2            // Beta��ϣ����ǰ���֡�Beta��������̽����
typedef unsigned long long ULL; // �޷��ų����ͣ����ڴ洢��ϣֵ

// Zobrist��ϣ��[��������][��][��] �� �����ϣֵ
ULL zobristTable[3][GRID_DIM][GRID_DIM];
ULL currentHash;                // ��ǰ����״̬�Ĺ�ϣֵ

// ��ϣ����Ŀ�ṹ���洢��̽����������״̬�������ظ����㣩
struct HashEntry {
    ULL key;    // ����״̬�Ĺ�ϣֵ������
    int depth;  // ��״̬̽��ʱ�����
    LLS score;  // ��״̬������
    int type;   // ��ϣ���ͣ�HASH_EXACT/HASH_ALPHA/HASH_BETA��
};
HashEntry* hashTable;           // ��ϣ��ָ�루��̬�����ڴ棩

// -------------------------- ����������ṹ���� --------------------------
#define MAX_MOVES 121          // �����������12x12���̹�144��Ԥ�����ࣩ

// ���νṹ��δʵ��ʹ�ã�Ԥ����չ�������ο��ӻ���
struct chessshape {
    int score;       // ��������
    char shape[9];   // �����ַ�����ʾ����"11101"���������ӣ�
};

// �������͵����ֽṹ
struct ChessStyle {
    LLS score;       // �����͵Ļ�������
    // ���캯����Ĭ������0��
    ChessStyle(int s = 0) : score(s) {}
};

// �������͵��������ã�AI����ֵ�����Ȩ�أ�
struct AllChessStyle {
    ChessStyle MyStyle[11];    // AI���������ֱ�
    ChessStyle EnemyStyle[11]; // ���ֵ��������ֱ���AIȨ��һ�£�

    // ���캯������ʼ���������֣�����Խ�ߣ��������ȼ�Խ�ߣ�
    AllChessStyle() {
        // ����0������Ч���ͣ�����0��
        MyStyle[0].score = 0;
        // ˯�����������Ӽ���հף���ֱ��������в��
        MyStyle[1].score = 10;
        // ����������������˿հף��ɷ�չΪ������
        MyStyle[2].score = 100;
        // ˯�����������Ӽ���հף��貹λ���ܳ��ģ�
        MyStyle[3].score = 100;
        // �����������������˿հף���һ���ɳɻ��ģ�
        MyStyle[4].score = 1050;
        // ���ģ��ĸ�����һ�˱�������һ�˿հף���һ���ɳ��壩
        MyStyle[5].score = 1000;
        // ���ģ��ĸ��������˿հף���һ���س��壩
        MyStyle[6].score = 1100000;
        // �������飨�ѻ�ʤ��������ȼ���
        MyStyle[7].score = 1000000000;
        // ���������������Ӽ��һ��հף���λ��ɻ�����
        MyStyle[8].score = 900;
        // ��˯�ģ��������Ӽ��һ��հף���λ��ɳ��ģ�
        MyStyle[9].score = 700;
        // �����ģ��������Ӽ��һ��հף���λ��ɻ��ģ�
        MyStyle[10].score = 1000;

        // ���ֵ�����������AIһ�£�STYLE_WEIGHT=1��
        for (int i = 0; i < 11; i++) {
            EnemyStyle[i].score = (LLS)MyStyle[i].score * 1;
        }
    }
};

// ����λ�ýṹ���洢�С������λ�õ����֣�
struct Position {
    int row = -1;    // �����꣨Ĭ��-1��������Чλ�ã�
    int col = -1;    // �����꣨Ĭ��-1��������Чλ�ã�
    LLS score = 0;   // ��λ�õ����֣����������ѡ���ӣ�

    // ���캯������ʼ���к��У�
    Position(int r = -1, int c = -1) {
        row = r;
        col = c;
    }
};

// ��ѡ�����б�ṹ���洢���п��ܵ�����λ�ã�
struct MoveList {
    Position moves[MAX_MOVES]; // ����λ������
    int count = 0;             // ��Ч����������ʵ�ʺ�ѡ����
};

// -------------------------- ���ߺ��� --------------------------
// ������������λ�ã����������ѡ���ӣ�
void swapPositions(Position& pos1, Position& pos2) {
    Position temp = pos1;
    pos1 = pos2;
    pos2 = temp;
}

// ����64λ����������ڳ�ʼ��Zobrist��ϣ��
ULL random64() {
    // ���3��32λ�����������64λ���ֵ
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

// -------------------------- Zobrist��ϣ���� --------------------------
// ���ҹ�ϣ����鵱ǰ����״̬�Ƿ���̽����
LLS lookupHash(ULL hash, int depth, LLS alpha, LLS beta) {
    // �����ϣ��������ȡģ����Խ�磩
    HashEntry& entry = hashTable[hash % HASH_SIZE];

    // ���ҵ�ƥ��Ĺ�ϣֵ����̽����ȡݵ�ǰ��ȣ���֤����ɿ���
    if (entry.key == hash && entry.depth >= depth) {
        if (entry.type == HASH_EXACT) {
            return entry.score; // ��ȷ���֣�ֱ�ӷ���
        }
        else if (entry.type == HASH_ALPHA && entry.score <= alpha) {
            return alpha;       // ���֡�Alpha����֦����̽��
        }
        else if (entry.type == HASH_BETA && entry.score >= beta) {
            return beta;        // ���֡�Beta����֦����̽��
        }
    }
    return MIN_VAL - 1; // δ�ҵ�ƥ�䣬������Чֵ
}

// �洢��ϣ������ǰ����״̬��̽����������ϣ��
void storeHash(ULL hash, int depth, LLS score, int type) {
    // �����ϣ������
    HashEntry& entry = hashTable[hash % HASH_SIZE];

    // ������ǰ��ȡ��Ѵ洢���ʱ���£���֤�����ε�̽���������ǳ�㣩
    if (entry.depth <= depth) {
        entry.key = hash;    // �洢��ϣֵ������
        entry.depth = depth; // �洢̽�����
        entry.score = score; // �洢����
        entry.type = type;   // �洢��ϣ����
    }
}

// -------------------------- ����״̬���� --------------------------
// ��Ϸ״̬�ṹ���洢���̡���Ϸ�׶Ρ���ϣֵ��
struct GameState {
    int grid[GRID_DIM][GRID_DIM] = { { 0 } }; // 12x12���̣���ʼȫ�հף�
    int gamePhase = 0;                        // ��Ϸ�׶Σ�δʵ��ʹ�ã�Ԥ����չ��
    ULL hash;                                 // ��ǰ���̵Ĺ�ϣֵ

    // ���캯������ʼ������
    GameState() {
        initializeBoard();
    }

    // ��ʼ�����̣�������̲����ó�ʼ��ϣֵ
    void initializeBoard() {
        // ������̣�����λ����Ϊ�հף�
        memset(grid, 0, sizeof(grid));
        // ��ʼ����ϣֵ����ʼΪ0��
        hash = 0;
        // �������̣������ʼ��ϣֵ������Ԥ���������ڴ˴����ã�
        for (int i = 0; i < GRID_DIM; i++) {
            for (int j = 0; j < GRID_DIM; j++) {
                if (grid[i][j] != BLANK) {
                    // ��ϣֵ = ���ǰλ�õ�Zobrist���ֵ
                    hash ^= zobristTable[grid[i][j]][i][j];
                }
            }
        }
    }

    // �������̣����Ӻ��������״̬���ϣֵ
    void updateGrid(int row, int col, int piece) {
        // 1. �Ƴ�ԭλ�����ӵĹ�ϣ���ף������ֵͬ������
        hash ^= zobristTable[grid[row][col]][row][col];
        // 2. ��������ӵĹ�ϣ����
        hash ^= zobristTable[piece][row][col];
        // 3. �������̸��ӵ���������
        grid[row][col] = piece;
    }
};

// ȫ�����̶�����������Ψһ������ʵ����
GameState gameBoard;

// -------------------------- ��ʼ������ --------------------------
// ��ʼ��Zobrist��ϣ������ÿ��(��������, ��, ��)�������ϣֵ
void initZobrist() {
    // ����������ӣ����ڵ�ǰʱ�䣬��֤ÿ�����й�ϣֵ��ͬ��
    srand((size_t)time(NULL));
    // ���������������ͣ�0=�հף�1=�ڣ�2=�ף�
    for (int piece = 0; piece < 3; piece++) {
        // �������������к���
        for (int i = 0; i < GRID_DIM; i++) {
            for (int j = 0; j < GRID_DIM; j++) {
                // Ϊÿ��(����, ��, ��)����64λ�����ϣֵ
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    // ��ʼ����ϣ����̬�����ڴ棩
    hashTable = new HashEntry[HASH_SIZE];
    // ��չ�ϣ��������Ŀ��ʼ��Ϊ0��
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);

    // �����ʼ���̵Ĺ�ϣֵ
    currentHash = 0;
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            if (gameBoard.grid[i][j] != BLANK) {
                currentHash ^= zobristTable[gameBoard.grid[i][j]][i][j];
            }
        }
    }
}

// -------------------------- ���ֺ��� --------------------------
// ���������֣����ĳ��λ����ָ�������ϵ����ͣ����������
int scoreDirects(GameState* state, Position pos, int dx, int dy, int player) {
    int count = 1;          // ��ǰ����������ʼΪ1������ǰλ�õ����ӣ�
    int empty_pos = 0;      // �հ�λ��״̬��1=���˿հף�2=���˿հף�
    int empty_count = 0;    // �հ�λ��������δʵ��ʹ�ã�
    int block = 0;          // ���赲���������˱����������赲�Ĵ�����
    // ��Ծ������ر�����������һ������ӣ���"101"��
    int jump_p = 0, jump_n = 0;        // ��/�������Ƿ������Ծ
    int count_j_p = 0, count_j_n = 0;  // ��/��������Ծ������
    bool block_j_p = 0, block_j_n = 0; // ��/��������Ծ���Ƿ񱻵�
    bool empty_j_p = 0, empty_j_n = 0; // ��/��������Ծ���Ƿ�հ�

    // -------------------------- �������飨�����ҡ����£� --------------------------
    int tx = pos.row + dx;  // ��ǰ�� + ����ƫ�ƣ�������
    int ty = pos.col + dy;  // ��ǰ�� + ����ƫ�ƣ�������
    bool found_empty = false; // �Ƿ��ҵ��հ�λ�ã������ж���Ծ��

    // ѭ��̽��������ֱ���������̱߽�
    while (tx >= 0 && tx < GRID_DIM && ty >= 0 && ty < GRID_DIM) {
        if (found_empty && !jump_p) {
            // ���1�����ҵ��հף���δ�����Ծ �� ����Ƿ�Ϊ��Ծ����
            if (state->grid[tx][ty] == player) {
                jump_p = 1;       // ���Ϊ��Ծ����
                count_j_p++;      // ��Ծ������+1
            }
            else {
                break;            // �Ǽ������ӣ�ֹͣ̽��
            }
        }
        else if (found_empty && jump_p) {
            // ���2�����ҵ��հ��ұ����Ծ �� ���������Ծ�������
            if (state->grid[tx][ty] == player) {
                count_j_p++;      // ��Ծ������+1
            }
            else if (state->grid[tx][ty] == 3 - player) {
                block_j_p = true; // �����������赲
                break;
            }
            else {
                empty_j_p = true; // ��Ծ��Ϊ�հ�
                break;
            }
        }
        else {
            // ���3��δ�ҵ��հ� �� �����������
            if (state->grid[tx][ty] == BLANK) {
                empty_count++;    // �հ���+1
                empty_pos += 1;   // ��ǵ��˿հ�
                found_empty = true; // ������ҵ��հ�
            }
            else if (state->grid[tx][ty] != player) {
                block++;          // �����������赲
                break;
            }
            else {
                count++;          // ��������������+1
            }
        }
        // �ƶ�����һ��λ�ã�������
        tx += dx;
        ty += dy;
    }

    // -------------------------- �������飨���������ϣ� --------------------------
    tx = pos.row - dx;  // ��ǰ�� - ����ƫ�ƣ�������
    ty = pos.col - dy;  // ��ǰ�� - ����ƫ�ƣ�������
    found_empty = false; // ���ÿհױ��

    // ѭ��̽��������ֱ���������̱߽�
    while (tx >= 0 && tx < GRID_DIM && ty >= 0 && ty < GRID_DIM) {
        if (found_empty && !jump_n) {
            // ���1�����ҵ��հף���δ�����Ծ �� �����Ծ����
            if (state->grid[tx][ty] == player) {
                jump_n = 1;       // ���Ϊ��Ծ����
                count_j_n++;      // ��Ծ������+1
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_n) {
            // ���2�����ҵ��հ��ұ����Ծ �� ���������Ծ�������
            if (state->grid[tx][ty] == player) {
                count_j_n++;      // ��Ծ������+1
            }
            else if (state->grid[tx][ty] == 3 - player) {
                block_j_n = true; // �������赲
                break;
            }
            else {
                empty_j_n = true; // ��Ծ��Ϊ�հ�
                break;
            }
        }
        else {
            // ���3��δ�ҵ��հ� �� �����������
            if (state->grid[tx][ty] == BLANK) {
                empty_count++;    // �հ���+1
                empty_pos += 2;   // ������˿հף�����������1���˴�+2 �� ��3��
                found_empty = true; // ������ҵ��հ�
            }
            else if (state->grid[tx][ty] != player) {
                block++;          // �������赲
                break;
            }
            else {
                count++;          // ��������������+1
            }
        }
        // �ƶ�����һ��λ�ã�������
        tx -= dx;
        ty -= dy;
    }

    // -------------------------- �����ж������� --------------------------
    // ������Ծ���ӣ����������������ģ�
    if (jump_p && jump_n) {
        // ˫����Ծ��δʵ�֣�Ԥ����չ��
    }
    else if (jump_p) {
        // ��������Ծ
        if (count + count_j_p == 3 && empty_pos == 3 && empty_j_p) {
            return 8; // ������
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && block_j_p) {
            return 9; // ��˯��
        }
        else if (count + count_j_p == 4 && empty_pos == 1 && empty_j_p) {
            return 9; // ��˯��
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && empty_j_p) {
            return 10; // ������
        }
    }
    else if (jump_n) {
        // ��������Ծ
        if (count + count_j_n == 3 && empty_pos == 3 && empty_j_n) {
            return 8; // ������
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && block_j_n) {
            return 9; // ��˯��
        }
        else if (count + count_j_n == 4 && empty_pos == 2 && empty_j_n) {
            return 9; // ��˯��
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && empty_j_n) {
            return 10; // ������
        }
    }

    // ������ͨ���ӣ�����Ծ��
    if (count >= 5) {
        return 7;  // �������飨��ʤ��
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return 6; // ���ģ����˿հף���Ӯ��
        }
        else if (empty_pos > 0) {
            return 5; // ���ģ�һ�˿հף��貹λ��
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return 4; // ���������˿հף��ɷ�չΪ���ģ�
        }
        else if (empty_pos > 0) {
            return 3; // ˯����һ�˿հף��貹λ���ܳ��ģ�
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return 2; // ��������˿հף��ɷ�չΪ������
        }
        else if (empty_pos > 0) {
            return 1; // ˯����һ�˿հף��貹λ���ܷ�չ��
        }
    }

    return 0; // ����Ч���ͣ�����0
}

// ����λ�����֣�����ĳλ�ö�ָ����ҵļ�ֵ��
LLS scorePlayer(GameState* state, Position pos, int player) {
    AllChessStyle styles;  // ������������
    LLS totalScore = 0;    // ������

    // ����ĸ���Ҫ�����¡��ҡ����¡����ϣ�������
    for (int i = 0; i < 4; i++) {
        // ��ȡ��ǰ������������ͣ�����������ģ�
        int styleType = scoreDirects(state, pos, dx[i], dy[i], player);
        // �ۼӸ����͵�����
        totalScore += styles.MyStyle[styleType].score;
    }

    // ������������������1400~1e6֮�䣬ǿ����Ϊ1e6�������и���в���͵����ȼ���
    if (totalScore >= 1400 && totalScore < 1000000) {
        totalScore = 1000000;
    }

    return totalScore;
}

// ��ѡλ�����֣�����ĳ�հ�λ�õ��ۺϼ�ֵ����AI����ֵ���в��
LLS scorePosts(GameState* state, Position pos) {
    AllChessStyle styles;  // ������������
    LLS totalScore = 0;    // ������
    LLS myScore = 0;       // AI�ڸ�λ�õ�����
    LLS rivalScore = 0;    // �����ڸ�λ�õ�����

    // ����ĸ���Ҫ���������
    for (int i = 0; i < 4; i++) {
        // ����AI�ڸ÷������������
        int myStyle = scoreDirects(state, pos, dx[i], dy[i], myColor);
        // ��������ڸ÷������������
        int rivalStyle = scoreDirects(state, pos, dx[i], dy[i], rivalColor);

        myScore += styles.MyStyle[myStyle].score;
        rivalScore += styles.EnemyStyle[rivalStyle].score;
    }

    // ����������ͬscorePlayer�����������и���в���ȼ���
    if (rivalScore >= 1400 && rivalScore < 1000000) {
        rivalScore = 1000000;
    }
    if (myScore >= 1400 && myScore < 1000000) {
        myScore = 1000000;
    }

    // �ۺ����� = AI���� + ������в���֣����ȷ��ض��ָ���в��
    totalScore += myScore + rivalScore;
    return totalScore;
}

// -------------------------- ��ѡ�������� --------------------------
// ���հ�λ���Ƿ������ӣ�����������Ч��ѡ���ӣ�����Ч�ʣ�
int hasNexter(GameState* state, int row, int col) {
    // 8�������������ҡ��ĸ��Խ��ߣ������2��Χ���Ƿ�������
    const int directions[8][2] = {
        { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 },
        { -1, -1 }, { -1, 1 }, { 1, -1 }, { 1, 1 }
    };

    for (int d = 0; d < 8; d++) {
        // ���1~2��Χ�ڵ�λ��
        for (int distance = 1; distance <= 2; distance++) {
            int ni = row + directions[d][0] * distance; // Ŀ����
            int nj = col + directions[d][1] * distance; // Ŀ����

            // ��Ŀ��λ�����������������ӣ��ǿհף�
            if (ni >= 0 && ni < GRID_DIM && nj >= 0 && nj < GRID_DIM && state->grid[ni][nj] != BLANK) {
                return 1; // �����ӣ���λ��Ϊ��Ч��ѡ
            }
        }
    }
    return 0; // �����ӣ��ų��ú�ѡ
}

// ���ɺ�ѡ�����б�ɸѡ�м�ֵ������λ�ã�����̽������
void getMoves(GameState* state, struct MoveList* moves, int player) {
    moves->count = 0;      // ���ú�ѡ��
    LLS value = 0;         // ����λ�õ�����
    int first_zero = 1;    // ����Ƿ�Ϊ��һ������0��λ�ã�����ȫ�հ�ʱ�޺�ѡ��

    // ������������
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            // ɸѡ�������հ�λ�� + �����ӣ�������Ч��ѡ��
            if (state->grid[i][j] == BLANK && hasNexter(state, i, j)) {
                Position tempPos = { i, j }; // ��ʱλ�ö���
                value = scorePosts(state, tempPos); // �����λ������

                if (value == 0 && first_zero) {
                    // ���1������0���ǵ�һ�� �� �����������޺�ѡ��
                    moves->moves[moves->count].row = i;
                    moves->moves[moves->count].col = j;
                    moves->moves[moves->count].score = value;
                    moves->count++;
                    first_zero = 0; // ��������0��λ�ò��ٱ���
                }
                else if (value > 0) {
                    // ���2������>0 �� �������м�ֵ�ĺ�ѡ��
                    moves->moves[moves->count].row = i;
                    moves->moves[moves->count].col = j;
                    moves->moves[moves->count].score = value;
                    moves->count++;
                }
            }
        }
    }

    // ��ѡ�������򣺰����ֽ�������̽���߼�ֵλ�ã�������֦Ч�ʣ�
    if (moves->count > 1) {
        stable_sort(moves->moves, moves->moves + moves->count,
            [](const Position& a, const Position& b) {
                return a.score > b.score;
            });
    }

    // ���ƺ�ѡ�������6��������̽���������ⳬʱ��
    if (moves->count > 6) {
        moves->count = 6;
    }
}

// -------------------------- Alpha-Beta��֦�㷨 --------------------------
// ���������֣����㵱ǰ���̶�AI���ۺϼ�ֵ��
LLS scoreBoard(GameState* state) {
    LLS comScore = 0;  // AI��������
    LLS humScore = 0;  // ���ֵ�������

    // ������������λ��
    for (int i = 0; i < GRID_DIM; i++) {
        for (int j = 0; j < GRID_DIM; j++) {
            Position p = { i, j };
            // �ۼ�AI�������ӵ�����
            if (state->grid[i][j] == myColor) {
                comScore += scorePlayer(state, p, myColor);
            }
            // �ۼӶ����������ӵ�����
            else if (state->grid[i][j] == rivalColor) {
                humScore += scorePlayer(state, p, rivalColor);
            }
        }
    }

    // ���̼�ֵ = AI���� - �������֣���ֵԽ�󣬶�AIԽ������
    return comScore - humScore;
}

// Alpha-Beta��֦���ĺ������ݹ�̽��δ�����ӣ�Ѱ�����Ž⣩
LLS AlphaBeta(GameState* state, int depth, LLS alpha, LLS beta, int player, Position lastMove) {
    // 1. ����ϣ������̽������״̬��ֱ�ӷ��ؽ������֦�Ż���
    LLS hashValue = lookupHash(state->hash, depth, alpha, beta);
    if (hashValue > MIN_VAL - 1) {
        return hashValue;
    }

    // 2. ����ʤ������һ�����ӵ��»�ʤ�����ؼ������֣���ֹ�ݹ飩
    if (player == myColor && scorePlayer(state, lastMove, rivalColor) >= 1000000000) {
        return MIN_VAL + 1; // ������һ����ʤ��AI����ͷ�
    }
    else if (player == rivalColor && scorePlayer(state, lastMove, myColor) >= 1000000000) {
        return MAX_VAL - 1; // AI��һ����ʤ������߷�
    }

    // 3. ̽�����Ϊ0�����ص�ǰ�������֣���ֹ�ݹ飩
    if (depth == 0) {
        LLS currentScore = scoreBoard(state);
        storeHash(state->hash, depth, currentScore, HASH_EXACT); // �洢��ϣ
        return currentScore;
    }

    // 4. ���ɺ�ѡ�����б�
    MoveList moves;
    getMoves(state, &moves, player);
    int hashType = HASH_ALPHA; // ��ʼ��ϣ���ͣ�Ĭ��Alpha��

    // 5. �޺�ѡ���ӣ����ص�ǰ�������֣�ƽ�ֻ���������
    if (moves.count == 0) {
        LLS currentScore = scoreBoard(state);
        storeHash(state->hash, depth, currentScore, HASH_EXACT);
        return currentScore;
    }

    // 6. �����ң�AI���ӣ�׷��������֣�
    if (player == myColor) {
        LLS maxEval = MIN_VAL; // ��ʼ���������

        // �������к�ѡ����
        for (int i = 0; i < moves.count; i++) {
            ULL oldHash = state->hash; // ���浱ǰ��ϣֵ�����ڻ��ݣ�

            // ģ�����ӣ������������ϣ
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, myColor);
            // �ݹ�̽�������-1���л�Ϊ�������ӣ�3-playerΪ������ɫ��
            LLS eval = AlphaBeta(state, depth - 1, alpha, beta, 3 - player, moves.moves[i]);
            // ���ݣ��ָ��������ϣ����Ӱ�����̽����
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
            state->hash = oldHash;

            // �������������Alphaֵ
            if (eval > maxEval) {
                maxEval = eval;
                if (eval > alpha) {
                    alpha = eval;
                    hashType = HASH_EXACT; // ���־�ȷ�����¹�ϣ����
                }
            }

            // Beta��֦������ǰ���֡�Beta�����ֲ���ѡ���·����ֹͣ̽��
            if (beta <= alpha) {
                hashType = HASH_BETA;
                break;
            }
        }

        // �洢��ǰ״̬�Ĺ�ϣ���
        storeHash(state->hash, depth, maxEval, hashType);
        return maxEval;
    }
    // 7. ��С����ң��������ӣ�׷��������֣�
    else {
        LLS minEval = MAX_VAL; // ��ʼ����С����

        // �������к�ѡ����
        for (int i = 0; i < moves.count; i++) {
            ULL oldHash = state->hash; // ���浱ǰ��ϣֵ

            // ģ���������
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, rivalColor);
            // �ݹ�̽�������-1���л�ΪAI����
            LLS eval = AlphaBeta(state, depth - 1, alpha, beta, 3 - player, moves.moves[i]);
            // ���ݣ��ָ��������ϣ
            state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
            state->hash = oldHash;

            // ������С������Betaֵ
            if (eval < minEval) {
                minEval = eval;
                if (eval < beta) {
                    beta = eval;
                    hashType = HASH_EXACT; // ���־�ȷ�����¹�ϣ����
                }
            }

            // Alpha��֦������ǰ���֡�Alpha��AI����ѡ���·����ֹͣ̽��
            if (beta <= alpha) {
                hashType = HASH_ALPHA;
                break;
            }
        }

        // �洢��ǰ״̬�Ĺ�ϣ���
        storeHash(state->hash, depth, minEval, hashType);
        return minEval;
    }
}

// Ѱ���������ӣ�������ѡ���ӣ�����Alpha-Beta�������Ž⣩
Position findBestShift(GameState* state) {
    // ��չ�ϣ��������һ��̽��������ţ�
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);

    // ����AI�ĺ�ѡ�����б�
    MoveList moves;
    getMoves(state, &moves, myColor);

    LLS bestScore = MIN_VAL; // �������֣���ʼΪ��Сֵ��
    Position bestMove(-1, -1); // ��������λ�ã���ʼΪ��Ч��

    // �������к�ѡ���ӣ�����ÿ��λ�õ�����
    for (int i = 0; i < moves.count; i++) {
        ULL oldHash = state->hash; // ���浱ǰ��ϣֵ

        // ģ��AI����
        state->updateGrid(moves.moves[i].row, moves.moves[i].col, myColor);
        // ����Alpha-Beta��������ӵĺ������֣�̽�����MAX_DEPTH=7��
        LLS score = AlphaBeta(state, 7, MIN_VAL, MAX_VAL, rivalColor, moves.moves[i]);
        // ���ݣ��ָ��������ϣ
        state->updateGrid(moves.moves[i].row, moves.moves[i].col, BLANK);
        state->hash = oldHash;

        // �����������ӣ����ָ������滻��
        if (score > bestScore) {
            bestScore = score;
            bestMove = moves.moves[i];
        }
    }

    return bestMove; // ������������λ��
}

// -------------------------- �������� --------------------------
// �������λ���Ƿ���Ч�������̷�Χ�ڣ�
bool isValidMove(Position pos) {
    return pos.row >= 0 && pos.col >= 0 && pos.row < GRID_DIM && pos.col < GRID_DIM;
}

// ��ӡ���̣�����ģʽ�¿��ӻ�����״̬��
void printBoard() {
    // ��ӡ�кţ�0~11��
    printf("    ");
    fflush(stdout); // ǿ��ˢ������������������ӳ٣�
    for (int col = 0; col < GRID_DIM; col++) {
        printf("%8d ", col);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);

    // ��ӡ�������ݣ��к�+���ӣ�
    for (int i = 0; i < GRID_DIM; i++) {
        // ��ӡ�зָ��ߣ���"+--------+--------+"��
        printf("   +");
        fflush(stdout);
        for (int j = 0; j < GRID_DIM; j++) {
            printf("--------+");
            fflush(stdout);
        }
        printf("\n%2d ", i); // ��ӡ�кţ�0~11��
        fflush(stdout);

        // ��ӡÿһ�е�����
        for (int j = 0; j < GRID_DIM; j++) {
            printf("|");
            fflush(stdout);
            if (gameBoard.grid[i][j] == myColor) {
                // ��ӡAI�����ӣ�����"��"������"��"��
                printf(myColor == PIECE_B ? "   ��   " : "   ��   ");
                fflush(stdout);
            }
            else if (gameBoard.grid[i][j] == rivalColor) {
                // ��ӡ���ֵ����ӣ���AI�෴��
                printf(rivalColor == PIECE_B ? "   ��   " : "   ��   ");
                fflush(stdout);
            }
            else {
                // �հ�λ�ã���ӡ�ո�
                printf("        ");
                fflush(stdout);
            }
        }
        printf("|\n"); // �н�����
        fflush(stdout);
    }

    // ��ӡ���̵ײ��ָ���
    printf("   +");
    fflush(stdout);
    for (int j = 0; j < GRID_DIM; j++) {
        printf("--------+");
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

// -------------------------- ��Ϸ��ѭ����ָ��� --------------------------
void gameLoop() {
    char command[10] = { 0 }; // �洢����ָ���"START"��"TURN"��
    Position movePos, currentPos; // movePos=��������λ�ã�currentPos=��ǰλ��
    Position myturn; // AI������λ��

    // ����ѭ������������ָ��
    while (1) {
        memset(command, 0, sizeof(command)); // ���ָ�����
        scanf("%s", command); // ��ȡָ��ӱ�׼���룩

        // -------------------------- �����ʼ��ָ�START�� --------------------------
        if (strcmp(command, CMD_INIT) == 0) {
            scanf("%d", &myColor); // ��ȡAI��������ɫ��1=�ڣ�2=�ף�
            rivalColor = 3 - myColor; // ���������ɫ����AI�෴��
            currentPos.row = CENTER_POS_1; // ��ʼλ����Ϊ���ģ�Ԥ����
            currentPos.col = (myColor == PIECE_B) ? CENTER_POS_2 : CENTER_POS_1;
            gameBoard.initializeBoard(); // ��ʼ������

            // ��Ӧָ����"OK"��������1������Ӧ��
            printf("OK\n");
            fflush(stdout);

            // ����ģʽ����ӡ��ʼ����
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- �������ģʽ����ָ�LIBERTY_STEP�� --------------------------
        else if (strcmp(command, CMD_BUGMODE) == 0) {
            bugmode = true; // ��������ģʽ
            // ��ӡ����ģʽ��ӭ��Ϣ
            printf("\n\n\n\n\n\n");
            printf("     WELCOME TO LIBERTY     \n");
            printf("\n\n\n\n\n\n");
        }

        // -------------------------- �����������ָ�PLACE�� --------------------------
        else if (strcmp(command, CMD_SET) == 0) {
            scanf("%d %d", &movePos.row, &movePos.col); // ��ȡ������������
            // �������̣��ڶ�������λ�÷��ö�������
            gameBoard.updateGrid(movePos.row, movePos.col, rivalColor);

            // ����ģʽ����ӡ���º������
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- ��������ֶ�����ָ�CRACK�� --------------------------
        else if (strcmp(command, CMD_CRACKER) == 0 && bugmode) {
            scanf("%d %d", &movePos.row, &movePos.col); // ��ȡ�ֶ���������
            // �������̣���ָ��λ�÷���AI����
            gameBoard.updateGrid(movePos.row, movePos.col, myColor);

            // ����ģʽ����ӡ���º������
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- ������Բ���ָ�ERASE�� --------------------------
        else if (strcmp(command, CMD_ERASE) == 0 && bugmode) {
            scanf("%d %d", &movePos.row, &movePos.col); // ��ȡ����λ������
            // �������̣���ָ��λ����Ϊ�հ�
            gameBoard.updateGrid(movePos.row, movePos.col, BLANK);

            // ����ģʽ����ӡ���º������
            if (bugmode) {
                printBoard();
            }
        }

        // -------------------------- ����AI����ָ�TURN�� --------------------------
        else if (strcmp(command, CMD_PLAY) == 0) {
            start_turn = clock(); // ��¼���ӿ�ʼʱ�䣨����ʱ�����Ƽ�飩
            Position nextMove; // �洢AI����������λ��

            // ���ú���Ѱ����������
            nextMove = findBestShift(&gameBoard);

            // ������λ����Ч��������겢��������
            if (isValidMove(nextMove)) {
                printf("%d %d\n", nextMove.row, nextMove.col); // �����������
                fflush(stdout); // ǿ��ˢ�£����ⳬʱ��
                // �������̣�����AI������
                gameBoard.updateGrid(nextMove.row, nextMove.col, myColor);

                // ����ģʽ����ӡ���º������
                if (bugmode) {
                    printBoard();
                }
            }
        }

        // -------------------------- ���������ʾ����ָ�SHOW�� --------------------------
        else if (strcmp(command, CMD_SHOW_BOARD) == 0 && bugmode) {
            printBoard(); // ��ӡ��ǰ����
        }

        // -------------------------- �������ʱ����ָ�LIMIT�� --------------------------
        else if (strcmp(command, CMD_LIMIT_CHECK) == 0 && bugmode) {
            bool failed = false; // ʱ�����Ƿ�ʧ�ܣ���ʱ��

            // ģ���غ����ӣ����ʱ�����ƣ�1900ms/����
            for (int i = 0; i < MAX_MOVES; i++) {
                auto start1 = clock(); // ��¼��ʼʱ��
                Position nextMove = findBestShift(&gameBoard); // Ѱ����������

                // ��������Ч����������
                if (isValidMove(nextMove)) {
                    fflush(stdout);
                    gameBoard.updateGrid(nextMove.row, nextMove.col, myColor);
                }

                // �����ʱ�����룩
                auto finish = clock();
                auto duration = (double)(finish - start1);

                // ����ʱ����1900ms�����ʧ�ܲ��˳�ѭ��
                if (duration > 1900) {
                    failed = true;
                    break;
                }

                swap(myColor, rivalColor); // �л���ɫ��ģ��������ӣ�
            }

            // ���ʱ������
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

// -------------------------- ��������������ڣ� --------------------------
int main() {
    initZobrist(); // ��ʼ��Zobrist��ϣ��
    gameLoop();    // ������Ϸ��ѭ��������ָ�
    return 0;      // ������������
}