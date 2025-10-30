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

bool hasNeighbor(int x, int y, int distance = 2);
// ���Կ��أ�1=���õ��������0=���õ������
#define DEBUG_MODE 0  // ʵ�ʶ�սʱ��Ϊ0���ɹر����е�����Ϣ

// ��������
const int BOARD_SIZE = 12;    // ���̴�С
const int BLACK = 1;          // ����
const int WHITE = 2;          // ����
const int EMPTY = 0;          // ��λ
const int MAX_SINGLE_TIME = 2; // �������ʱ��(��)
const int MAX_TOTAL_TIME = 90; // ��ʱ������(��)
const int HASH_SIZE = 1000003; // ��ϣ���С(����)
using ULL = unsigned long long;
using LLS = long long;

// �������ã���������������������
const int SCORE_FIVE = 1000000000;    // ������(ʤ��)
const int SCORE_LIVE_FOUR = 1100000; // ����(���˿�)
const int SCORE_SLEEP_FOUR = 700; // ����(���˿�)
const int SCORE_JUMP_LIVE_FOUR = 1000; // ������
const int SCORE_RUSH_FOUR = 1000;  // ����(һ�˿�)
const int SCORE_JUMP_RUSH_FOUR = 900;  // ������
const int SCORE_LIVE_THREE = 1050;   // ����(���˿�)
const int SCORE_JUMP_LIVE_THREE = 900;  // ������
const int SCORE_SLEEP_THREE = 100;   // ����(һ�˿�)
const int SCORE_LIVE_TWO = 10;      // ���
const int SCORE_SLEEP_TWO = 5;      // �߶�

// ��ϣ����ض���
enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

// ��ϣ��ṹ
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
HashEntry* hashTable;          // ����ϣ��
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

    // ��ʼ����ϣ���ڴ�
    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

// ���ҹ�ϣ��
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

// �洢��ϣ��
void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    // ֻ�洢�����ε��������
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

// ��������������ؿ��ƣ�
void debug(const char* message) {
#if DEBUG_MODE == 1  // ����DEBUG_MODEΪ1ʱ�����������Ϣ
    printf("DEBUG %s\n", message);
    fflush(stdout);
#endif
}

// ��ʼ�����̺͹�ϣ��ʼֵ
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    board[5][5] = 2;
    board[6][6] = 2;
    board[6][5] = 2;
    board[5][6] = 2;
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

// ��������״̬�͹�ϣֵ
void update_board(int x, int y, int color) {
    // �Ƴ�ԭλ�õĹ�ϣֵ��ԭ״̬�ǿգ�
    currentHash ^= zobristTable[EMPTY][x][y];
    // ��������ӵĹ�ϣֵ
    currentHash ^= zobristTable[color][x][y];
    // ��������״̬
    board[x][y] = color;
}

// ����ָ��λ����ĳ�������������,��KAMI���Ӧ��û����
int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;
    int count = 1;          // ��ǰλ�õ�����������
    int empty_pos = 0;      // ��λ״̬(1=һ�˿�,3=���˿�)
    int block = 0;          // ���赲��

    // ��������������"1 0 1"�ȼ�����ӣ�
    int jump_p = 0, jump_n = 0;        // ��/�������Ƿ�������
    int count_j_p = 0, count_j_n = 0;  // ����������
    bool block_j_p = false, block_j_n = false; // �����Ƿ񱻵�
    bool empty_j_p = false, empty_j_n = false; // �����Ƿ��п�λ

    // -------------------------- �����⣨�ط�������� --------------------------
    int x = posx + dx;
    int y = posy + dy;
    bool found_empty = false;

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_p) {
            // �ҵ���λ�󣬼���Ƿ�Ϊ��������"1 0 1"��
            if (board[x][y] == player) {
                jump_p = 1;
                count_j_p++;
            }
            else {
                break;
            }
        }
        else if (found_empty && jump_p) {
            // ������������Ӽ��
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
            // δ�ҵ���λ�����������
            if (board[x][y] == EMPTY) {
                empty_pos += 1;  // һ�˿�
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // ������+1
            }
        }
        x += dx;
        y += dy;
    }

    // -------------------------- �����⣨�ط���ݼ��� --------------------------
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
                empty_pos += 2;  // ���˿գ��ܺ�Ϊ3��
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;  // ������+1
            }
        }
        x -= dx;
        y -= dy;
    }

    // // -------------------------- �����ж������� --------------------------
    // ������Ծ���ӣ����������������ģ�
    if (jump_p) {
        // ��������Ծ
        if (count + count_j_p == 3 && empty_pos == 3 && empty_j_p) {
            return SCORE_JUMP_LIVE_THREE; // ������
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && block_j_p) {
            return SCORE_JUMP_RUSH_FOUR; // ��˯��
        }
        else if (count + count_j_p == 4 && empty_pos == 1 && empty_j_p) {
            return SCORE_JUMP_RUSH_FOUR; // ��˯��
        }
        else if (count + count_j_p == 4 && empty_pos == 3 && empty_j_p) {
            return SCORE_JUMP_LIVE_FOUR; // ������
        }
    }
    else if (jump_n) {
        // ��������Ծ
        if (count + count_j_n == 3 && empty_pos == 3 && empty_j_n) {
            return SCORE_JUMP_LIVE_THREE; // ������
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && block_j_n) {
            return SCORE_RUSH_FOUR; // ��˯��
        }
        else if (count + count_j_n == 4 && empty_pos == 2 && empty_j_n) {
            return SCORE_RUSH_FOUR; // ��˯��
        }
        else if (count + count_j_n == 4 && empty_pos == 3 && empty_j_n) {
            return SCORE_JUMP_LIVE_FOUR; // ������
        }
    }

    // ������ͨ���ӣ�����Ծ��
    if (count >= 5) {
        return SCORE_FIVE;  // �������飨��ʤ��
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR; // ���ģ����˿հף���Ӯ��
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR; // ���ģ�һ�˿հף��貹λ��
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE; // ���������˿հף��ɷ�չΪ���ģ�
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE; // ˯����һ�˿հף��貹λ���ܳ��ģ�
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO; // ��������˿հף��ɷ�չΪ������
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO; // ˯����һ�˿հף��貹λ���ܷ�չ��
        }
    }

    return 0; // ����Ч���ͣ�����0 ����ж��߼� -------------------------
}

// ����ָ��λ�õ��������֣���ʱ���Ӻ���㣩
LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    //// ��ʱ���ӣ���������
    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    LLS myScore = 0;       // AI�ڸ�λ�õ�����
    LLS rivalScore = 0;    // �����ڸ�λ�õ�����
    // 4������ˮƽ(0,1)����ֱ(1,0)����б(1,1)����б(1,-1)
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // �����ҷ����ӵķ���
    for (auto& dir : dirs) {
        myScore += scoreDirects(x, y, dir[0], dir[1], player);
    }

    // ����Է����ӵķ�����Ȩ�ص�����
    for (auto& dir : dirs) {
        rivalScore += scoreDirects(x, y, dir[0], dir[1], opponent); // ����Ȩ��
    }
    // ����������ͬscorePlayer�����������и���в���ȼ���
    if (rivalScore >= 1400 && rivalScore < 1000000) {
        rivalScore = 1000000;
    }
    if (myScore >= 1400 && myScore < 1000000) {
        myScore = 1000000;
    }
    total += myScore + rivalScore;
    //// ������ʱ����
    update_board(x, y, EMPTY);
    return total;
}


// ������ǰ���̵÷�
LLS evaluate() {
    LLS totalScore = 0;
    // ������������λ�ü������
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                // �ۼ��ҷ����ӵĵ÷�
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                // ��ȥ�Է����ӵĵ÷֣�����Ȩ�أ�
                totalScore -= scorePosition(i, j, board[i][j]) ;
            }
        }
    }
    return totalScore;
}

// ����ϣ�Ż���Alpha-Beta����
LLS alphaBeta(int depth, LLS alpha, LLS beta) {
    // �ȼ���ϣ��
    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    // �����������ȣ���������ֵ
    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    // �������п��ܵ��߷�,����������������,��Ҫ������ѡ6��
    vector<pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_pos(i, j) && hasNeighbor(i, j, 2)) {
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
        bool isWin = scorePosition(x,y, currentColor);

        LLS score;
        if (isWin) {
            // ʤ���ø߷�
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            // �ݹ�����
            score = -alphaBeta(depth - 1, -beta, -alpha);
        }

        // �������Ӻ͹�ϣ
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

    // �洢��ϣ��
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

pair<int, int> get_best_move() {
    // ���ȷ����Է���ʱʤ����в
    /*int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY);
                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }*/

    // ���ɺ�ѡ���Ӳ���������
    vector<pair<LLS, pair<int, int>>> candidateMoves;
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            // ����������λ����Ч �� 1-2����������������
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 2)) {
                LLS score = scorePosition(x, y, my_color);
                candidateMoves.emplace_back(score, make_pair(x, y));
            }
        }
    }

    // �����ֽ����������ȸ߼�ֵλ�ã�
    // ��ԭ������������޸�Ϊ��
    sort(candidateMoves.begin(), candidateMoves.end(),
        [](const pair<LLS, pair<int, int>>& a, const pair<LLS, pair<int, int>>& b) {
            return a.first > b.first;
        });

    // ���ƺ�ѡ���������6��������̽������
    int maxCandidates = 6;
    if (candidateMoves.size() > maxCandidates) {
        candidateMoves.resize(maxCandidates);
    }

    // ʹ��Alpha-Beta�����Ӻ�ѡ��ѡ�����Ž�
    int bestX = -1, bestY = -1;
    LLS bestScore = -20000000000;
    int searchDepth = 4;

    for (const auto& candidate : candidateMoves) {
        int x = candidate.second.first;
        int y = candidate.second.second;

        update_board(x, y, my_color);
        bool isWin = scorePosition(x, y, my_color);
        LLS score;
        score = -alphaBeta(searchDepth - 1, -1e18, 1e18);        

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    // ��ѡ�����ҵ���Чλ��
    if (bestX != -1) {
        debug(("Optimal move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback����
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 1)) {
                debug(("Near enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    for (int x = 4; x <= 8; x++) {
        for (int y = 4; y <= 8; y++) {
            if (is_valid_pos(x, y)) {
                return { x, y };
            }
        }
    }

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
        update_board(x, y, enemy_color); // �������̺͹�ϣ
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

// ���ָ��λ����Χdistance��Χ���Ƿ�������
bool hasNeighbor(int x, int y, int distance ) {
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            if (dx == 0 && dy == 0) continue; // ��������

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                if (board[nx][ny] != EMPTY) {
                    return true;
                }
            }
        }
    }
    return false;
}

// ��ӡ��Ч��ѡ�߷��������ã�
void printValidMoves() {
#if DEBUG_MODE == 1  // ������ģʽ�´�ӡ
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
#endif
}

// ����TURN����
void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves(); // ������ģʽ�´�ӡ

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
        update_board(x, y, my_color); // �������̺͹�ϣ
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