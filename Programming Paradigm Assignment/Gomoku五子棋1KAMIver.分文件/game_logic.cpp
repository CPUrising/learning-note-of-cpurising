#include "constants.h"
#include "structures.h"
#include "hash.cpp"
#include <algorithm>
#include <cstring>
#include <ctime>

// ȫ�ֱ���
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

// ʵ�ֽṹ�巽��
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

// ���ߺ���
void swapPositions(Position& pos1, Position& pos2) {
    Position temp = pos1;
    pos1 = pos2;
    pos2 = temp;
}

bool isEven2(int number) {
    return number % 2 == 0;
}

// ���ֺ���
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

// �ƶ�����������
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

int partitionMoves(Position moveArr[], int startIdx, int endIdx, GameState* state, int player) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}

void quickSortMoves(Position moveArr[], int startIdx, int endIdx, GameState* state, int player) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}

bool comparePositions(const Position& a, const Position& b) {
    return a.score > b.score;
}

void getMoves(GameState* state, MoveList* moves, int player) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}

// AI�����㷨
LLS scoreBoard(GameState* state) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}

LLS AlphaBeta(GameState* state, int depth, LLS alpha, LLS beta, int player, Position lastMove) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}

Position findBestShift(GameState* state) {
    // ʵ��ԭ�����е��߼�
    // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
}