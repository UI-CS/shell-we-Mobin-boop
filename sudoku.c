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
void *check_grid(void *p) {
    param *data = p;
    int used[10] = {0};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int num = sudoku[data->row+i][data->col+j];
            if (used[num]) {
                valid[data->index] = 0;
                pthread_exit(NULL);
            }
            used[num] = 1;
        }
    valid[data->index] = 1;
    pthread_exit(NULL);
}
int main() {
    pthread_t threads[11];
    param data[11];
    int t = 0;

    data[t] = (param){0,0,t};
    pthread_create(&threads[t], NULL, check_row, &data[t]);
    t++;

    data[t] = (param){0,0,t};
    pthread_create(&threads[t], NULL, check_col, &data[t]);
    t++;

    int idx = 2;
    for (int i = 0; i < 9; i += 3)
        for (int j = 0; j < 9; j += 3) {
            data[idx] = (param){i,j,idx};
            pthread_create(&threads[idx], NULL, check_grid, &data[idx]);
            idx++;
        }

    for (int i = 0; i < 11; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < 11; i++)
        if (valid[i] == 0) {
            printf("Sudoku NOT ok\n");
            return 0;
        }

    printf("Sudoku ok\n");
    return 0;
}
