#include "constants.h"
#include "structures.h"
#include "game_logic.cpp"
#include <cstdio>
#include <cstring>

// ����ƶ��Ƿ�Ϸ�
bool isValidMove(Position pos) {
    return pos.row >= 0 && pos.col >= 0 && pos.row < GRID_DIM && pos.col < GRID_DIM;
}

// ��ӡ����
void printBoard() {
    printf("    ");
    fflush(stdout);
    for (int col = 0; col < GRID_DIM; col++) {
        printf("%8d ", col);
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
    for (int i = 0; i < GRID_DIM; i++) {
        printf("   +");
        fflush(stdout);
        for (int j = 0; j < GRID_DIM; j++) {
            printf("--------+");
            fflush(stdout);
        }
        printf("\n%2d ", i);
        fflush(stdout);
        for (int j = 0; j < GRID_DIM; j++) {
            printf("|");
            fflush(stdout);
            if (gameBoard.grid[i][j] == myColor) {
                printf(myColor == PIECE_B ? "   ��   " : "   ��   ");
                fflush(stdout);
            }
            else if (gameBoard.grid[i][j] == rivalColor) {
                printf(rivalColor == PIECE_B ? "   ��   " : "   ��   ");
                fflush(stdout);
            }
            else {
                printf("        ");
                fflush(stdout);
            }
        }
        printf("|\n");
        fflush(stdout);
    }
    printf("   +");
    fflush(stdout);
    for (int j = 0; j < GRID_DIM; j++) {
        printf("--------+");
        fflush(stdout);
    }
    printf("\n");
    fflush(stdout);
}

// ��Ϸѭ��
void gameLoop() {
    char command[10] = { 0 };
    Position movePos, currentPos;
    Position myturn;
    while (1) {
        memset(command, 0, sizeof(command));
        scanf("%s", command);

        if (strcmp(command, CMD_INIT) == 0) {
            // �����ʼ������
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_BUGMODE) == 0) {
            // �������ģʽ����
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_SET) == 0) {
            // ����������������
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_CRACKER) == 0 && bugmode) {
            // �����ƽ�����
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_ERASE) == 0 && bugmode) {
            // �����������
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_PLAY) == 0) {
            // ������Ϸ����
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_SHOW_BOARD) == 0 && bugmode) {
            // ������ʾ��������
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
        else if (strcmp(command, CMD_LIMIT_CHECK) == 0 && bugmode) {
            // �������Ƽ������
            // ...��ʡ�Ծ���ʵ�֣�������ԭ����һ�£�
        }
    }
}