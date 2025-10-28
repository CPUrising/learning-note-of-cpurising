#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>
#include <cstdlib>
#include <algorithm>
using namespace std;

// ��������
const int BOARD_SIZE = 12;    // ���̳ߴ�
const int BLACK = 1;          // ����
const int WHITE = 2;          // ����
const int EMPTY = 0;          // ��λ
const int MAX_SINGLE_TIME = 2; // �������ʱ��(��)
const int MAX_TOTAL_TIME = 90; // �����ʱ��(��)
const int HASH_SIZE = 1000003; // ��ϣ���С(����)
using ULL = unsigned long long;
using LLS = long long;

// �������ֳ�������ֵ�ɸ��ݲ��Ե�����
const int SCORE_FIVE = 1000000;    // �������飨��ʤ��
const int SCORE_LIVE_FOUR = 100000; // ���ģ����˿հף���Ӯ��
const int SCORE_JUMP_LIVE_FOUR = 90000; // ������
const int SCORE_RUSH_FOUR = 10000;  // ���ģ�һ�˿հף�
const int SCORE_JUMP_RUSH_FOUR = 9000;  // �����ģ���˯�ģ�
const int SCORE_LIVE_THREE = 1000;   // ���������˿հף�
const int SCORE_JUMP_LIVE_THREE = 900;  // ������
const int SCORE_SLEEP_THREE = 100;   // ˯����һ�˿հף�
const int SCORE_LIVE_TWO = 10;      // ���
const int SCORE_SLEEP_TWO = 5;      // ˯��

// ��ϣ����Ŀ����
enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

// ��ϣ����Ŀ�ṹ
struct HashEntry {
    ULL key;       // ��ϣֵ(��)
    int depth;     // �������
    LLS score;     // ��������
    HashType type; // ��Ŀ����
};

// ȫ�ֱ���
vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;              // AI������ɫ
clock_t start_total_time;      // ��Ϸ��ʼʱ��
ULL zobristTable[3][BOARD_SIZE][BOARD_SIZE]; // Zobrist��ϣ��
HashEntry* hashTable;          // �����ϣ��
ULL currentHash = 0;           // ��ǰ���̹�ϣֵ

// ����64λ�����
ULL random64() {
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

// ��ʼ��Zobrist��ϣ��
void initZobrist() {
    srand((unsigned int)time(NULL));
    for (int piece = 0; piece < 3; piece++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    // ��ʼ����ϣ�����
    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

// ���ҹ�ϣ����
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

// �洢��ϣ����
void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    // ֻ�洢�������������
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

// �������
void debug(const char* message) {
    printf("DEBUG %s\n", message);
    fflush(stdout);
}

// ��ʼ�����̣�������ϣ��ʼ����
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    // ��ʼ����ǰ��ϣֵ������λ��Ϊ�գ�
    currentHash = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            currentHash ^= zobristTable[EMPTY][i][j];
        }
    }
    debug("Board initialized successfully");
}

// ���λ����Ч��
bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;
    }
    return board[x][y] == EMPTY;
}

// �������̲�ͬ����ϣֵ
void update_board(int x, int y, int color) {
    // ���Ƴ�ԭλ�õĹ�ϣֵ��ԭ���ǿգ�
    currentHash ^= zobristTable[EMPTY][x][y];
    // ��������ӵĹ�ϣֵ
    currentHash ^= zobristTable[color][x][y];
    // ��������״̬
    board[x][y] = color;
}

// �������������λ��(posx, posy)�ڷ���(dx, dy)�ϵ���������
int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;
    int count = 1;          // ��ǰλ�õ����Ӽ���������
    int empty_pos = 0;      // �հ�״̬��1=���˿գ�3=���˿գ�
    int block = 0;          // ���赲����

    // ��Ծ������ر���������"101"�ȼ�����ӣ�
    int jump_p = 0, jump_n = 0;        // ��/�������Ƿ�����Ծ
    int count_j_p = 0, count_j_n = 0;  // ��Ծ������
    bool block_j_p = false, block_j_n = false; // ��Ծ���Ƿ񱻵�
    bool empty_j_p = false, empty_j_n = false; // ��Ծ���Ƿ�հ�

    // -------------------------- �������飨�����ҡ����£� --------------------------
    int x = posx + dx;
    int y = posy + dy;
    bool found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_p) {
            // ���ҵ��հף�����Ƿ�Ϊ��Ծ���ӣ���"1 0 1"��
            if (board[x][y] == player) {
                jump_p = 1;
                count_j_p++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_p) {
            // ��Ծ��������
            if (board[x][y] == player) {
                count_j_p++;
            }
            else if (board[x][y] == opponent) {
                block_j_p = true;
                break;
            }
            else {
                empty_j_p = true;
                break;
            }
        }
        else {
            // δ�ҵ��հף������������
            if (board[x][y] == EMPTY) {
                empty_pos += 1;  // ���˿հ�
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // ��������+1
            }
        }
        x += dx;
        y += dy;
    }

    // -------------------------- �������飨���������ϣ� --------------------------
    x = posx - dx;
    y = posy - dy;
    found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_n) {
            if (board[x][y] == player) {
                jump_n = 1;
                count_j_n++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_n) {
            if (board[x][y] == player) {
                count_j_n++;
            }
            else if (board[x][y] == opponent) {
                block_j_n = true;
                break;
            }
            else {
                empty_j_n = true;
                break;
            }
        }
        else {
            if (board[x][y] == EMPTY) {
                empty_pos += 2;  // ���˿հף��ۼ�Ϊ3��
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // ��������+1
            }
        }
        x -= dx;
        y -= dy;
    }

    // -------------------------- �����ж������� --------------------------
    // ������Ծ����
    if (jump_p || jump_n) {
        int total_jump = count + max(count_j_p, count_j_n);
        if (total_jump == 4) {
            if ((jump_p && empty_j_p && empty_pos == 3) || (jump_n && empty_j_n && empty_pos == 3)) {
                return SCORE_JUMP_LIVE_FOUR; // ������
            }
            else if ((jump_p && (block_j_p || empty_j_p)) || (jump_n && (block_j_n || empty_j_n))) {
                return SCORE_JUMP_RUSH_FOUR; // ������
            }
        }
        else if (total_jump == 3 && empty_pos == 3) {
            return SCORE_JUMP_LIVE_THREE; // ������
        }
    }

    // ������ͨ���ӣ�����Ծ��
    if (count >= 5) {
        return SCORE_FIVE; // ��������
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR; // ���ģ����˿գ�
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR; // ���ģ�һ�˿գ�
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE; // ���������˿գ�
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE; // ˯����һ�˿գ�
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO; // ��������˿գ�
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO; // ˯����һ�˿գ�
        }
    }

    return 0; // ����Ч����
}

// ����ָ��λ�����Ӻ���ۺ����֣����ֵ��ң�
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    // ��ʱ���ӣ��������ּ���
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    // 4�����򣺺�(0,1)����(1,0)��б��(1,1)��б��(1,-1)
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // ���㼺�����͵÷�
    for (auto& dir : dirs) {
        total += scoreDirects(x, y, dir[0], dir[1], player);
    }

    // ����з����͵÷֣�Ȩ�ؿɵ�����ͨ����Ҫ���ȷ��أ�
    for (auto& dir : dirs) {
        total -= scoreDirects(x, y, dir[0], dir[1], opponent) * 0.9; // ����Ȩ���Ե�
    }

    // ������ʱ����
    update_board(x, y, EMPTY);
    return total;
}
// ����Ƿ��ʤ
bool check_win(int x, int y, int color) {
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1;

        // ������
        for (int i = 1; i < 5; i++) {
            int nx = x + dx * i;
            int ny = y + dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
                count++;
            }
            else {
                break;
            }
        }

        // ������
        for (int i = 1; i < 5; i++) {
            int nx = x - dx * i;
            int ny = y - dy * i;
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[nx][ny] == color) {
                count++;
            }
            else {
                break;
            }
        }

        if (count >= 5) {
            return true;
        }
    }
    return false;
}

// ����������ʾ����
// �µ�����������������������
LLS evaluate() {
    LLS totalScore = 0;
    // ���������зǿ�λ�ý�������
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                // �ۼӼ����������ӵ����ͷ�
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                // ��ȥ�з��������ӵ����ͷ֣����أ�
                totalScore -= scorePosition(i, j, board[i][j]) * 0.9;
            }
        }
    }
    return totalScore;
}

// ����ϣ������������򻯰�Alpha-Beta��
LLS alphaBeta(int depth, LLS alpha, LLS beta) {
    // �ȼ���ϣ����
    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    // ���������ȣ���������ֵ
    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    // �������п��ܵ�����
    vector<pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_pos(i, j)) {
                moves.emplace_back(i, j);
            }
        }
    }

    if (moves.empty()) {
        LLS score = 0; // ƽ��
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    LLS bestScore = -1e18;
    int currentColor = (depth % 2 == 0) ? my_color : (my_color == BLACK ? WHITE : BLACK);

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        // ���Ӳ����¹�ϣ
        update_board(x, y, currentColor);
        bool isWin = check_win(x, y, currentColor);

        LLS score;
        if (isWin) {
            // ��ʤ�ø߷�
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            // �ݹ�����
            score = -alphaBeta(depth - 1, -beta, -alpha);
        }

        // �������Ӳ��ָ���ϣ
        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
        }
        if (bestScore > alpha) {
            alpha = bestScore;
        }
        if (alpha >= beta) {
            break; // Beta��֦
        }
    }

    // �洢��ϣ���
    HashType type;
    if (bestScore <= alpha) {
        type = HASH_ALPHA;
    }
    else if (bestScore >= beta) {
        type = HASH_BETA;
    }
    else {
        type = HASH_EXACT;
    }
    storeHash(currentHash, depth, bestScore, type);

    return bestScore;
}

// ��ȡ������ӣ�ʹ�ù�ϣ�����Ż���
pair<int, int> get_best_move() {
    // ����Ƿ��бط�λ�ã������߼���
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY); // �ָ�

                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // ʹ�ô���ϣ����������㷨
    int bestX = -1, bestY = -1;
    LLS bestScore = -1e18;
    int searchDepth = 3; // �������

    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                // ��������
                update_board(x, y, my_color);
                bool isWin = check_win(x, y, my_color);
                LLS score;

                if (isWin) {
                    score = 1e18; // ��ʤ�߷�
                }
                else {
                    // �������ֵ����Ӧ��
                    score = -alphaBeta(searchDepth - 1, -1e18, 1e18);
                }

                // �ָ�����
                update_board(x, y, EMPTY);

                if (score > bestScore) {
                    bestScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }
    }

    // �����������Чλ��
    if (bestX != -1) {
        debug(("Hash-optimized move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback�߼�������λ��
    for (int x = 5; x <= 7; x++) {
        for (int y = 5; y <= 7; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Occupy center at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    // ����Ĭ���߷�
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Default move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    return { -1, -1 };
}

// ����START����
void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock();
    debug(("Start game with color: " + to_string(my_color)).c_str());
    printf("OK\n");
    fflush(stdout);
}

// ����PLACE����
void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        update_board(x, y, enemy_color); // ʹ�ô���ϣ���µĺ���
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

// ���ָ��λ����Χ1-2�����Ƿ�������
bool hasNeighbor(int x, int y, int distance = 2) {
    // �����Χdistance��Χ�ڵ�����λ��
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            // ������ǰλ������
            if (dx == 0 && dy == 0) continue;

            int nx = x + dx;
            int ny = y + dy;

            // ����Ƿ������̷�Χ����������
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board[nx][ny] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ��ȡ������Ч�ĺ�ѡ����λ��
vector<pair<int, int>> getMoves() {
    vector<pair<int, int>> validMoves;
    // ֻ��������������Χ2���ڵ�λ�ã�������Ч���㣩
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y)) {
                validMoves.emplace_back(x, y);
            }
        }
    }

    // ��û����Ч�߷���Ĭ��������
    if (validMoves.empty()) {
        for (int x = 5; x <= 7; x++) {
            for (int y = 5; y <= 7; y++) {
                if (is_valid_pos(x, y)) {
                    validMoves.emplace_back(x, y);
                }
            }
        }
    }

    // �����������߷���Alpha-Beta��֦Ч�ʸ��ߣ�
    sort(validMoves.begin(), validMoves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
        return scorePosition(a.first, a.second, my_color) > scorePosition(b.first, b.second, my_color);
        });

    return validMoves;
}

// ��ӡ������Ч��ѡ����λ�ã�������֤ɸѡ�߼�
void printValidMoves() {
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
}
// ����TURN����
void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves();
    // �����ʱ��
    clock_t current_time = clock();
    int used = (current_time - start_total_time) / CLOCKS_PER_SEC;
    if (used > MAX_TOTAL_TIME) {
        debug(("Total time exceeded: " + to_string(used) + "s").c_str());
        return;
    }

    // ��ȡ����߷�
    pair<int, int> move = get_best_move();
    int x = move.first, y = move.second;

    // ��鵥��ʱ��
    int single_used = (clock() - start_single_time) / CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return;
    }

    // ִ���߷�
    if (is_valid_pos(x, y)) {
        update_board(x, y, my_color); // ʹ�ô���ϣ���µĺ���
        printf("%d %d\n", x, y);
        fflush(stdout);
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug("Invalid move generated!");
        // Ѱ�ҵ�һ����λ
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (is_valid_pos(i, j)) {
                    update_board(i, j, my_color);
                    printf("%d %d\n", i, j);
                    fflush(stdout);
                    return;
                }
            }
        }
    }
}

int main() {
    initZobrist(); // ��ʼ��Zobrist��ϣ��
    debug("Brain program started");
    char cmd[20];

    while (true) {
        if (scanf("%s", cmd) != 1) {
            continue;
        }

        if (strcmp(cmd, "START") == 0) {
            int color;
            scanf("%d", &color);
            handle_start(color);
        }
        else if (strcmp(cmd, "PLACE") == 0) {
            int x, y;
            scanf("%d %d", &x, &y);
            handle_place(x, y);
        }
        else if (strcmp(cmd, "TURN") == 0) {
            handle_turn();
        }
        else if (strcmp(cmd, "END") == 0) {
            int winner;
            scanf("%d", &winner);
            string win_msg = (winner == 0) ? "Draw game" :
                (winner == my_color) ? "I won" : "Enemy won";
            debug(win_msg.c_str());
            break;
        }
        else {
            debug(("Unknown command: " + string(cmd)).c_str());
        }
    }

    delete[] hashTable; // �ͷŹ�ϣ���ڴ�
    return 0;
}