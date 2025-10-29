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
vector<pair<int, int>> getMoves();

#define DEBUG_MODE 1

const int BOARD_SIZE = 12;    // �������̳ߴ�����Ӧ����ս��
const int BLACK = 1;
const int WHITE = 2;
const int EMPTY = 0;
const int MAX_SINGLE_TIME = 2;
const int MAX_TOTAL_TIME = 90;
const int HASH_SIZE = 1000003;
using ULL = unsigned long long;
using LLS = long long;

// ��������Ȩ�أ���ǿ��вʶ�����ȼ�
const int SCORE_FIVE = 1000000;
const int SCORE_LIVE_FOUR = 100000;
const int SCORE_JUMP_LIVE_FOUR = 90000;
const int SCORE_RUSH_FOUR = 20000;  // ��߳��ķ���
const int SCORE_JUMP_RUSH_FOUR = 18000;
const int SCORE_LIVE_THREE = 5000;  // ��߻�������
const int SCORE_JUMP_LIVE_THREE = 4500;
const int SCORE_SLEEP_THREE = 1000;
const int SCORE_LIVE_TWO = 100;
const int SCORE_SLEEP_TWO = 50;

enum HashType { HASH_EXACT, HASH_ALPHA, HASH_BETA };

struct HashEntry {
    ULL key;
    int depth;
    LLS score;
    HashType type;
};

vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;
clock_t start_total_time;
ULL zobristTable[3][BOARD_SIZE][BOARD_SIZE];
HashEntry* hashTable;
ULL currentHash = 0;
int threat_count = 0;  // ������в������

ULL random64() {
    return ((ULL)rand() << 32) | ((ULL)rand() << 16) | (ULL)rand();
}

void initZobrist() {
    srand((unsigned int)time(NULL));
    for (int piece = 0; piece < 3; piece++) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                zobristTable[piece][i][j] = random64();
            }
        }
    }

    hashTable = new HashEntry[HASH_SIZE];
    memset(hashTable, 0, sizeof(HashEntry) * HASH_SIZE);
}

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

void storeHash(ULL hash, int depth, LLS score, HashType type) {
    HashEntry& entry = hashTable[hash % HASH_SIZE];
    if (entry.depth <= depth) {
        entry.key = hash;
        entry.depth = depth;
        entry.score = score;
        entry.type = type;
    }
}

void debug(const char* message) {
#if DEBUG_MODE == 1
    printf("DEBUG %s\n", message);
    fflush(stdout);
#endif
}

void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    currentHash = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            currentHash ^= zobristTable[EMPTY][i][j];
        }
    }
    debug("Board initialized successfully");
}

bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;
    }
    return board[x][y] == EMPTY;
}

void update_board(int x, int y, int color) {
    currentHash ^= zobristTable[EMPTY][x][y];
    currentHash ^= zobristTable[color][x][y];
    board[x][y] = color;
}

// ����������Ƿ��γ����ģ�������в������
bool is_four_in_row(int x, int y, int color) {
    int dirs[4][2] = { {0,1},{1,0},{1,1},{1,-1} };
    for (auto& dir : dirs) {
        int count = 1;
        // �������
        for (int i = 1; i <= 4; i++) {  // ��Ϊ<=4��ȷ���ܼ�⵽����4�ӵ����
            int nx = x + dir[0] * i;
            int ny = y + dir[1] * i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[nx][ny] != color)
                break;
            count++;
        }
        // �������
        for (int i = 1; i <= 4; i++) {  // ͬ��
            int nx = x - dir[0] * i;
            int ny = y - dir[1] * i;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE || board[nx][ny] != color)
                break;
            count++;
        }
        // ����������4��ʱ���ж�Ϊ������в
        if (count == 4) {
            // ��������Ƿ��п�λ��ȷ���ǳ��Ķ��������ģ�
            int end1_x = x + dir[0] * (count - 1);
            int end1_y = y + dir[1] * (count - 1);
            int end2_x = x - dir[0] * (count - 1);
            int end2_y = y - dir[1] * (count - 1);
            bool has_empty = false;
            if (is_valid_pos(end1_x + dir[0], end1_y + dir[1])) has_empty = true;
            if (is_valid_pos(end2_x - dir[0], end2_y - dir[1])) has_empty = true;
            if (has_empty) return true;
        }
        // ��������ֱ���ж�ʤ��
        if (count >= 5) return true;
    }
    return false;
}

int scoreDirects(int posx, int posy, int dx, int dy, int player) {
    int opponent = (player == BLACK) ? WHITE : BLACK;  // ������ɫ
    int count = 1;          // �����ļ�������������ʼΪ1��������ǰλ�ã�
    int empty_pos = 0;      // ��¼�����ϵĿ�λ״̬�������жϡ��/���ߡ���
    int block = 0;          // ��¼�����ϵ��赲�����������ӻ�߽磩

    // ��������ر�����������������ӣ��硰���������������ġ���
    int jump_p = 0, jump_n = 0;
    int count_j_p = 0, count_j_n = 0;
    bool block_j_p = false, block_j_n = false;
    bool empty_j_p = false, empty_j_n = false;

    // -------------------------- 1. �������������ӵ�ǰλ����(dx, dy)���죩--------------------------
    int x = posx + dx;  // �ӵ�ǰλ�õ���һ�����ӿ�ʼ����
    int y = posy + dy;
    bool found_empty = false;  // ����Ƿ���������λ

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {  // ���������̱߽�
        if (found_empty && !jump_p) {
            // ���1����������λ����δ�����������Ρ������������������ӣ��ж�Ϊ�����塱
            if (board[x][y] == player) {
                jump_p = 1;          // ��������������
                count_j_p++;         // �������������+1
            }
            else {
                break;  // �����������ӻ�߽磬ֹͣ����
            }
        }
        else if (found_empty && jump_p) {
            // ���2���Ѵ����������Ρ����������ж����������״̬
            if (board[x][y] == player) {
                count_j_p++;         // ��������������+1
            }
            else if (board[x][y] == opponent) {
                block_j_p = true;    // ���屻�����赲
                break;
            }
            else {
                empty_j_p = true;    // ����ĩ���п�λ�������죩
                break;
            }
        }
        else {
            // ���3��δ������λ������������������
            if (board[x][y] == EMPTY) {
                empty_pos += 1;      // ��¼��λ�������λ���Ϊ1��
                found_empty = true;  // �����������λ
            }
            else if (board[x][y] == opponent) {
                block++;             // �����������ӣ��赲��+1
                break;
            }
            else {
                count++;             // �����������ӣ���������+1
            }
        }
        x += dx;  // �����������ƶ�
        y += dy;
    }

    // -------------------------- 2. �������������ӵ�ǰλ����(-dx, -dy)���죩--------------------------
    x = posx - dx;  // �ӵ�ǰλ�õ�ǰһ�����ӿ�ʼ����
    y = posy - dy;
    found_empty = false;  // ���ÿ�λ���

    while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        if (found_empty && !jump_n) {
            // �������߼�һ�£�������ġ������Ρ�
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
            // δ������λ����������
            if (board[x][y] == EMPTY) {
                empty_pos += 2;      // �����λ���Ϊ2�����������֣����ں����жϣ�
                found_empty = true;
            }
            else if (board[x][y] == opponent) {
                block++;
                break;
            }
            else {
                count++;
            }
        }
        x -= dx;  // ���������ƶ�
        y -= dy;
    }

    // -------------------------- 3. ���������֣�������������ӣ�--------------------------
    if (jump_p || jump_n) {
        int total_jump = count + max(count_j_p, count_j_n);  // ������������������+�������ӣ�
        // �����ģ��硰������𡱣��м��һ�����˿����죩
        if (total_jump == 4) {
            if ((jump_p && empty_j_p && empty_pos == 3) || (jump_n && empty_j_n && empty_pos == 3)) {
                return SCORE_JUMP_LIVE_FOUR;  // �����ķ����������ȼ���
            }
            else if ((jump_p && (block_j_p || empty_j_p)) || (jump_n && (block_j_n || empty_j_n))) {
                return SCORE_JUMP_RUSH_FOUR;  // �����ķ������и����ȼ���
            }
        }
        // ���������硰�����𡱣��м��һ�񵫿��γɻ�����
        else if (total_jump == 3 && empty_pos == 3) {
            return SCORE_JUMP_LIVE_THREE;  // �����������������ȼ���
        }
    }

    // -------------------------- 4. ��ͨ�����������֣������������ӣ�--------------------------
    if (count >= 5) {
        return SCORE_FIVE;  // ���壨ֱ�ӻ�ʤ��������ȼ���
    }
    else if (count == 4) {
        if (empty_pos == 3) {
            return SCORE_LIVE_FOUR;  // ���ģ����˾��п�λ����ֱ�ӳ��壬�����ȼ���
        }
        else if (empty_pos > 0) {
            return SCORE_RUSH_FOUR;  // ���ģ�һ���п�λ���貹һ�ӳ��壬�и����ȼ���
        }
    }
    else if (count == 3) {
        if (empty_pos == 3) {
            return SCORE_LIVE_THREE;  // �����������п�λ���ɷ�չΪ���ģ������ȼ���
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_THREE;  // ������һ�˱�������չ���ޣ������ȼ���
        }
    }
    else if (count == 2) {
        if (empty_pos == 3) {
            return SCORE_LIVE_TWO;  // ����������п�λ���ɷ�չΪ�����������ȼ���
        }
        else if (empty_pos > 0) {
            return SCORE_SLEEP_TWO;  // �߶���һ�˱�������չ���ޣ��������ȼ���
        }
    }

    return 0;  // ����Ч���Σ��絥�ӡ����ӣ���ս����ֵ��
}

LLS scorePosition(int x, int y, int player) {
    if (!is_valid_pos(x, y)) return 0;

    update_board(x, y, player);

    int opponent = (player == BLACK) ? WHITE : BLACK;
    LLS total = 0;
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    // ����Ƿ��γ���������в
    bool is_four = is_four_in_row(x, y, player);
    if (is_four) {
        total += SCORE_RUSH_FOUR * 0.5;  // ����ӷ�
    }

    for (auto& dir : dirs) {
        total += scoreDirects(x, y, dir[0], dir[1], player);
    }

    for (auto& dir : dirs) {
        total -= scoreDirects(x, y, dir[0], dir[1], opponent) * 0.9;
    }

    update_board(x, y, EMPTY);
    return total;
}

bool check_win(int x, int y, int color) {
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1;
        //��������
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

// ������ͳ�Ƶ�ǰ��в����
void count_threats() {
    threat_count = 0;
    int enemy = (my_color == BLACK) ? WHITE : BLACK;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == enemy) {
                if (is_four_in_row(i, j, enemy)) {
                    threat_count++;
                }
            }
        }
    }
}

LLS evaluate() {
    LLS totalScore = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == my_color) {
                totalScore += scorePosition(i, j, my_color);
            }
            else if (board[i][j] != EMPTY) {
                totalScore -= scorePosition(i, j, board[i][j]) * 0.9;
            }
        }
    }
    return totalScore;
}

LLS alphaBeta(int depth, LLS alpha, LLS beta, int max_depth) {
    // �������ݹ���ȣ���ֹ��ʱ
    if (depth >= max_depth) {
        LLS score = evaluate();
        return score;
    }

    LLS hashScore = lookupHash(currentHash, depth, alpha, beta);
    if (hashScore != -2000000000000000001LL) {
        return hashScore;
    }

    if (depth == 0) {
        LLS score = evaluate();
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    vector<pair<int, int>> moves = getMoves();  // ʹ���Ż���ĺ�ѡ��
    if (moves.empty()) {
        LLS score = 0;
        storeHash(currentHash, depth, score, HASH_EXACT);
        return score;
    }

    LLS bestScore = -1e18;
    int currentColor = (depth % 2 == 0) ? my_color : (my_color == BLACK ? WHITE : BLACK);

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        update_board(x, y, currentColor);
        bool isWin = check_win(x, y, currentColor);

        LLS score;
        if (isWin) {
            score = (currentColor == my_color) ? 1e18 : -1e18;
        }
        else {
            score = -alphaBeta(depth - 1, -beta, -alpha, max_depth);
        }

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
        }
        if (bestScore > alpha) {
            alpha = bestScore;
        }
        if (alpha >= beta) {
            break;
        }
    }

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
    count_threats();  // ��ͳ����в����
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;

    // 1. ���ȷ�����ֱ�ӻ�ʤ��λ��
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                bool enemy_win = check_win(x, y, enemy_color);
                update_board(x, y, EMPTY);//to understand
                
                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 2. ���س��ĺ���������в����ǿ�棩
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                update_board(x, y, enemy_color);
                LLS enemy_score = 0;
                int dirs[4][2] = { {0,1},{1,0},{1,1},{1,-1} };
                for (auto& dir : dirs) {
                    enemy_score += scoreDirects(x, y, dir[0], dir[1], enemy_color);
                }
                // ����Ƿ��γ�������в,��TMû���ð�
                if (is_four_in_row(x, y, enemy_color)) {
                    enemy_score += SCORE_RUSH_FOUR;
                }
                update_board(x, y, EMPTY);

                if (enemy_score >= SCORE_RUSH_FOUR) {
                    debug(("Block enemy rush four at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 3. ������в�̶ȶ�̬�����������
    int searchDepth = 4;
    if (threat_count >= 2) {
        searchDepth = 6;  // ����вʱ�����������
        debug("Increased search depth due to multiple threats");
    }

    // 4. ��������Ƶ�Alpha-Beta����
    int bestX = -1, bestY = -1;
    LLS bestScore = -1e18;
    vector<pair<int, int>> moves = getMoves();

    for (auto& move : moves) {
        int x = move.first, y = move.second;
        update_board(x, y, my_color);
        bool isWin = check_win(x, y, my_color);
        LLS score;

        if (isWin) {
            score = 1e18;
        }
        else {
            score = -alphaBeta(searchDepth - 1, -1e18, 1e18, searchDepth);
        }

        update_board(x, y, EMPTY);

        if (score > bestScore) {
            bestScore = score;
            bestX = x;
            bestY = y;
        }
    }

    if (bestX != -1) {
        debug(("Hash-optimized move at (" + to_string(bestX) + "," + to_string(bestY) + ")").c_str());
        return { bestX, bestY };
    }

    //  fallback�߼����ֲ���
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 1)) {
                debug(("Near enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    for (int x = 7; x <= 8; x++) {  // �����������Ϊ��������
        for (int y = 7; y <= 8; y++) {
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

void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock();
    debug(("Start game with color: " + to_string(my_color)).c_str());
    printf("OK\n");
    fflush(stdout);
}

void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        update_board(x, y, enemy_color);
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

bool hasNeighbor(int x, int y, int distance) {
    for (int dx = -distance; dx <= distance; dx++) {
        for (int dy = -distance; dy <= distance; dy++) {
            if (dx == 0 && dy == 0) continue;

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

int getDynamicDepth(clock_t start_single_time) {
    clock_t current = clock();
    double elapsed = (current - start_single_time) / (double)CLOCKS_PER_SEC;
    int base_depth = 3;

    // ����ʣ��ʱ�䶯̬�������
    if (elapsed < 0.5) {
        return (threat_count >= 2) ? 5 : 4;
    }
    else if (elapsed < 1.0) {
        return (threat_count >= 2) ? 4 : 3;
    }
    else {
        return 2; // ʱ�����ʱʹ��ǳ������
    }
}
vector<pair<int, int>> getMoves() {
    vector<pair<int, int>> validMoves;
    // �����ھӼ�ⷶΧ��3�񣬼���©��
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y) && hasNeighbor(x, y, 2)) {
                validMoves.emplace_back(x, y);
            }
        }
    }

    if (validMoves.empty()) {
        for (int x = 7; x <= 8; x++) {
            for (int y = 7; y <= 8; y++) {
                if (is_valid_pos(x, y)) {
                    validMoves.emplace_back(x, y);
                }
            }
        }
    }

    // �Ż����򣬽�Ϸ���Ȩ��
    sort(validMoves.begin(), validMoves.end(), [&](const pair<int, int>& a, const pair<int, int>& b) {
        int enemy = (my_color == BLACK) ? WHITE : BLACK;
        LLS scoreA = scorePosition(a.first, a.second, my_color) +
            scorePosition(a.first, a.second, enemy) * 0.5;  // ���ӷ���Ȩ��
        LLS scoreB = scorePosition(b.first, b.second, my_color) +
            scorePosition(b.first, b.second, enemy) * 0.5;
        return scoreA > scoreB;
        });
    if (validMoves.size() > 20) {
        validMoves.resize(20);
    }
    return validMoves;
}

void printValidMoves() {
#if DEBUG_MODE == 1
    vector<pair<int, int>> moves = getMoves();
    printf("Valid moves count: %d\n", moves.size());
    for (auto& move : moves) {
        printf("(%d, %d) ", move.first, move.second);
    }
    printf("\n");
    fflush(stdout);
#endif
}

void handle_turn() {
    clock_t start_single_time = clock();
    printValidMoves();

    // ֻ��ȡһ������߷�
    pair<int, int> best_move = get_best_move();
    int x = best_move.first, y = best_move.second;

    // ���ʱ���Ƿ�ʱ
    double single_used = (clock() - start_single_time) / (double)CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return;
    }

    if (is_valid_pos(x, y)) {
        update_board(x, y, my_color);
        printf("%d %d\n", x, y);
        fflush(stdout);
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        //  fallback�߼�
        debug("Invalid move generated!");
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
    initZobrist();
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

    delete[] hashTable;
    return 0;
}