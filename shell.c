#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define HISTORY_SIZE 5

char history[HISTORY_SIZE][MAX_LINE];
int history_count = 0;

void add_history(char *cmd) {
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count], cmd);
        history_count++;
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++)
            strcpy(history[i - 1], history[i]);
        strcpy(history[HISTORY_SIZE - 1], cmd);
    }
}

void parse(char *input, char **args) {
    int i = 0;
    char *token = strtok(input, " \n");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL;
}

int is_background(char **args) {
    int i = 0;
    while (args[i] != NULL) i++;
    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        args[i - 1] = NULL;
        return 1;
    }
    return 0;
}

int has_pipe(char **args) {
    for (int i = 0; args[i] != NULL; i++)
        if (strcmp(args[i], "|") == 0)
            return i;
    return -1;
}
int main() {
    char *line;
    char *args[MAX_ARGS];
    int running = 1;

    while (running) {
        while (waitpid(-1, NULL, WNOHANG) > 0);

        printf("uinxsh> ");
        line = malloc(MAX_LINE);
        if (!fgets(line, MAX_LINE, stdin)) {
            free(line);
            break;
        }

