#include <stdio.h>
#include <pthread.h>

int sudoku[9][9] = {
    {5,3,4,6,7,8,9,1,2},
    {6,7,2,1,9,5,3,4,8},
    {1,9,8,3,4,2,5,6,7},
    {8,5,9,7,6,1,4,2,3},
    {4,2,6,8,5,3,7,9,1},
    {7,1,3,9,2,4,8,5,6},
    {9,6,1,5,3,7,2,8,4},
    {2,8,7,4,1,9,6,3,5},
    {3,4,5,2,8,6,1,7,9}
};

int valid[11];

typedef struct {
    int row;
    int col;
    int index;
} param;

void *check_row(void *p) {
    param *data = p;
    int used[10] = {0};
    for (int i = 0; i < 9; i++) {
        int num = sudoku[data->row][i];
        if (used[num]) {
            valid[data->index] = 0;
            pthread_exit(NULL);
        }
        used[num] = 1;
    }
    valid[data->index] = 1;
    pthread_exit(NULL);
}

void *check_col(void *p) {
    param *data = p;
    int used[10] = {0};
    for (int i = 0; i < 9; i++) {
        int num = sudoku[i][data->col];
        if (used[num]) {
            valid[data->index] = 0;
            pthread_exit(NULL);
        }
        used[num] = 1;
    }
    valid[data->index] = 1;
    pthread_exit(NULL);
}
