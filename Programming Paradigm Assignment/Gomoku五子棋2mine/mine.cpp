//2458620 古贺 终于后悔反省了
#include"head.h"
// 全局常量定义
const int BOARD_SIZE = 12;    // 12×12棋盘
const int BLACK = 1;          // 黑棋（己方）
const int WHITE = 2;          // 白棋（对方）
const int EMPTY = 0;          // 空位置
const int MAX_SINGLE_TIME = 2; // 单步最大时间（秒）
const int MAX_TOTAL_TIME = 90; // 总最大时间（秒）

// 全局变量（棋盘状态、己方颜色、时间统计）
vector<vector<int>> board(BOARD_SIZE, vector<int>(BOARD_SIZE, EMPTY));
int my_color = 0;              // 己方颜色（1=黑，2=白）
clock_t start_total_time;      // 对局开始时间（统计总用时）
int total_used_time = 0;       // 已使用总时间（秒）

// 调试输出函数（符合文档要求：格式为DEBUG[MESSAGE]，独占一行）
void debug(const char* message) {
    printf("DEBUG %s\n", message);
    fflush(stdout); // 强制刷新缓冲区，避免输出延迟
}

// 初始化棋盘（清空所有位置）
void init_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = EMPTY;
        }
    }
    debug("Board initialized successfully");
}

// 检查坐标合法性（是否在棋盘内且为空）
bool is_valid_pos(int x, int y) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false; // 超出棋盘范围
    }
    return board[x][y] == EMPTY; // 位置是否为空
}

// 检查某位置落子后是否获胜（横向/纵向/对角线，连续5子）
bool check_win(int x, int y, int color) {
    // 方向数组：右、下、右下、右上（4个主方向，避免重复判断）
    int dirs[4][2] = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

    for (auto& dir : dirs) {
        int dx = dir[0], dy = dir[1];
        int count = 1; // 当前位置已落子，计数为1

        // 正向统计（如向右、向下）
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

        // 反向统计（如向左、向上）
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

        // 连续5子及以上则获胜
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

// 基础落子策略（优先级：1.防守对方即将获胜的位置 2.占据中心及附近位置）
pair<int, int> get_best_move() {
    // 第一步：防守对方即将获胜的位置（优先堵对方4子连线）
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                // 模拟对方落子，检查是否获胜
                board[x][y] = (my_color == BLACK) ? WHITE : BLACK;
                bool enemy_win = check_win(x, y, (my_color == BLACK) ? WHITE : BLACK);
                board[x][y] = EMPTY; // 撤销模拟

                if (enemy_win) {
                    debug(("Defend at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                    return { x, y };
                }
            }
        }
    }

    // 第二步：若无防守需求，优先占据中心区域（5-7行/列，12×12棋盘中心）
    for (int x = 5; x <= 7; x++) {
        for (int y = 5; y <= 7; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Occupy center at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    // 第三步：若无中心位置，遍历棋盘找第一个空位置（保底策略）
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (is_valid_pos(x, y)) {
                debug(("Default move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
                return { x, y };
            }
        }
    }

    return { -1, -1 }; // 理论上不会触发（棋盘满时对局已结束）
}

// 处理START指令（初始化颜色和棋盘，1秒内响应OK）
void handle_start(int color) {
    my_color = color;
    init_board();
    start_total_time = clock(); // 记录对局开始时间
    debug(("Start game with color: " + to_string(my_color)).c_str());

    // 1秒内响应OK（文档要求）
    printf("OK\n");
    fflush(stdout);
}

// 处理PLACE指令（更新对方落子到棋盘）
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

// 处理TURN指令（计算落子，2秒内输出坐标）
void handle_turn() {
    clock_t start_single_time = clock(); // 记录单步开始时间

    // 1. 检查总用时是否超限（文档要求：总用时<90秒）
    clock_t current_time = clock();
    int used = (current_time - start_total_time) / CLOCKS_PER_SEC;
    if (used > MAX_TOTAL_TIME) {
        debug(("Total time exceeded: " + to_string(used) + "s").c_str());
        return; // 超限会被平台判负
    }

    // 2. 计算最佳落子（基础策略）
    pair<int, int> move = get_best_move();
    int x = move.first, y = move.second;

    // 3. 检查单步用时是否超限（文档要求：单步<2秒）
    int single_used = (clock() - start_single_time) / CLOCKS_PER_SEC;
    if (single_used > MAX_SINGLE_TIME) {
        debug(("Single move time exceeded: " + to_string(single_used) + "s").c_str());
        return; // 超限会被平台判负
    }

    // 4. 输出落子坐标（文档要求：独占一行，空格分隔）
    if (is_valid_pos(x, y)) {
        board[x][y] = my_color; // 更新己方落子到棋盘
        printf("%d %d\n", x, y);
        fflush(stdout); // 强制刷新，避免延迟
        debug(("My move at (" + to_string(x) + "," + to_string(y) + ")").c_str());
    }
    else {
        debug("Invalid move generated!");
        // 保底：输出第一个空位置（避免格式错误判负）
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

// 主函数（指令循环解析）
int main() {
    debug("Brain program started");
    char cmd[20]; // 存储指令（如START、PLACE、TURN、END）

    while (true) {
        // 读取指令（避免scanf阻塞：仅在有数据时读取，文档注意事项1）
        if (scanf("%s", cmd) != 1) {
            continue;
        }

        // 解析START指令（格式：START [FIELD]，FIELD=1/2）
        if (strcmp(cmd, "START") == 0) {
            int color;
            scanf("%d", &color);
            handle_start(color);
        }

        // 解析PLACE指令（格式：PLACE X Y，X/Y为对方落子坐标）
        else if (strcmp(cmd, "PLACE") == 0) {
            int x, y;
            scanf("%d %d", &x, &y);
            handle_place(x, y);
        }

        // 解析TURN指令（轮到己方落子）
        else if (strcmp(cmd, "TURN") == 0) {
            handle_turn();
        }

        // 解析END指令（对局结束，无需响应，文档要求）
        else if (strcmp(cmd, "END") == 0) {
            int winner;
            scanf("%d", &winner);
            string win_msg = (winner == 0) ? "Draw game" :
                (winner == my_color) ? "I won" : "Enemy won";
            debug(win_msg.c_str());
            break; // 退出程序（平台会强制关闭，文档要求）
        }

        // 未知指令（调试用）
        else {
            debug(("Unknown command: " + string(cmd)).c_str());
        }
    }

    return 0;
}