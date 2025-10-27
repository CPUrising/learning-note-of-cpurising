//2458620 �ź� ���ں�ڷ�ʡ��
#include"head.h"
// ȫ�ֳ�������
const int BOARD_SIZE = 12;    // 12��12����
const int BLACK = 1;          // ���壨������
const int WHITE = 2;          // ���壨�Է���
const int EMPTY = 0;          // ��λ��
const int MAX_SINGLE_TIME = 2; // �������ʱ�䣨�룩
const int MAX_TOTAL_TIME = 90; // �����ʱ�䣨�룩

// ȫ�ֱ���������״̬��������ɫ��ʱ��ͳ�ƣ�
vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;              // ������ɫ��1=�ڣ�2=�ף�
clock_t start_total_time;      // �Ծֿ�ʼʱ�䣨ͳ������ʱ��
int total_used_time = 0;       // ��ʹ����ʱ�䣨�룩

// ������������������ĵ�Ҫ�󣺸�ʽΪDEBUG[MESSAGE]����ռһ�У�
void debug(const char* message) {
    printf("DEBUG %s\n", message);
    fflush(stdout); // ǿ��ˢ�»���������������ӳ�
}

// ��ʼ�����̣��������λ�ã�
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    debug("Board initialized successfully");
}

// �������Ϸ��ԣ��Ƿ�����������Ϊ�գ�
bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false; // �������̷�Χ
    }
    return board[x][y] == EMPTY; // λ���Ƿ�Ϊ��
}

// ���ĳλ�����Ӻ��Ƿ��ʤ������/����/�Խ��ߣ�����5�ӣ�
bool check_win(int x, int y, int color) {
    // �������飺�ҡ��¡����¡����ϣ�4�������򣬱����ظ��жϣ�
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1; // ��ǰλ�������ӣ�����Ϊ1

        // ����ͳ�ƣ������ҡ����£�
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

        // ����ͳ�ƣ����������ϣ�
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

        // ����5�Ӽ��������ʤ
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

// �������Ӳ��ԣ����ȼ���1.���ضԷ�������ʤ��λ�� 2.ռ�����ļ�����λ�ã�
pair<int, int> get_best_move() {
    // ��һ�������ضԷ�������ʤ��λ�ã����ȶ¶Է�4�����ߣ�
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                // ģ��Է����ӣ�����Ƿ��ʤ
                board[x][y] = (my_color == BLACK) ? WHITE : BLACK;
                bool enemy_win = check_win(x, y, (my_color == BLACK) ? WHITE : BLACK);
                board[x][y] = EMPTY; // ����ģ��

                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // �ڶ��������޷�����������ռ����������5-7��/�У�12��12�������ģ�
    for (int x = 5; x <= 7; x++) {
        for (int y = 5; y <= 7; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Occupy center at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    // ����������������λ�ã����������ҵ�һ����λ�ã����ײ��ԣ�
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Default move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    return { -1, -1 }; // �����ϲ��ᴥ����������ʱ�Ծ��ѽ�����
}

// ����STARTָ���ʼ����ɫ�����̣�1������ӦOK��
void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock(); // ��¼�Ծֿ�ʼʱ��
    debug(("Start game with color: " + to_string(my_color)).c_str());

    // 1������ӦOK���ĵ�Ҫ��
    printf("OK\n");
    fflush(stdout);
}

// ����PLACEָ����¶Է����ӵ����̣�
void handle_place(int x, int y) {
    int enemy_color = (my_color == BLACK) ? WHITE : BLACK;
    if (is_valid_pos(x, y)) {
        board[x][y] = enemy_color;
        debug(("Enemy placed at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug(("Invalid enemy move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
}

// ����TURNָ��������ӣ�2����������꣩
void handle_turn() {
    clock_t start_single_time = clock(); // ��¼������ʼʱ��

    // 1. �������ʱ�Ƿ��ޣ��ĵ�Ҫ������ʱ<90�룩
    clock_t current_time = clock();
    int used = (current_time - start_total_time) / CLOCKS_PER_SEC;
    if (used > MAX_TOTAL_TIME) {
        debug(("Total time exceeded: " + to_string(used) + "s").c_str());
        return; // ���޻ᱻƽ̨�и�
    }

    // 2. ����������ӣ��������ԣ�
    pair<int, int> move = get_best_move();
    int x = move.first, y = move.second;

    // 3. ��鵥����ʱ�Ƿ��ޣ��ĵ�Ҫ�󣺵���<2�룩
    int single_used = (clock() - start_single_time) / CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return; // ���޻ᱻƽ̨�и�
    }

    // 4. ����������꣨�ĵ�Ҫ�󣺶�ռһ�У��ո�ָ���
    if (is_valid_pos(x, y)) {
        board[x][y] = my_color; // ���¼������ӵ�����
        printf("%d %d\n", x, y);
        fflush(stdout); // ǿ��ˢ�£������ӳ�
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug("Invalid move generated!");
        // ���ף������һ����λ�ã������ʽ�����и���
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (is_valid_pos(i, j)) {
                    board[i][j] = my_color;
                    printf("%d %d\n", i, j);
                    fflush(stdout);
                    return;
                }
            }
        }
    }
}

// ��������ָ��ѭ��������
int main() {
    debug("Brain program started");
    char cmd[20]; // �洢ָ���START��PLACE��TURN��END��

    while (true) {
        // ��ȡָ�����scanf����������������ʱ��ȡ���ĵ�ע������1��
        if (scanf("%s", cmd) != 1) {
            continue;
        }

        // ����STARTָ���ʽ��START [FIELD]��FIELD=1/2��
        if (strcmp(cmd, "START") == 0) {
            int color;
            scanf("%d", &color);
            handle_start(color);
        }

        // ����PLACEָ���ʽ��PLACE X Y��X/YΪ�Է��������꣩
        else if (strcmp(cmd, "PLACE") == 0) {
            int x, y;
            scanf("%d %d", &x, &y);
            handle_place(x, y);
        }

        // ����TURNָ��ֵ��������ӣ�
        else if (strcmp(cmd, "TURN") == 0) {
            handle_turn();
        }

        // ����ENDָ��Ծֽ�����������Ӧ���ĵ�Ҫ��
        else if (strcmp(cmd, "END") == 0) {
            int winner;
            scanf("%d", &winner);
            string win_msg = (winner == 0) ? "Draw game" :
                (winner == my_color) ? "I won" : "Enemy won";
            debug(win_msg.c_str());
            break; // �˳�����ƽ̨��ǿ�ƹرգ��ĵ�Ҫ��
        }

        // δָ֪������ã�
        else {
            debug(("Unknown command: " + string(cmd)).c_str());
        }
    }

    return 0;
}