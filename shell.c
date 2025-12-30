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
        if (strcmp(line, "!!\n") == 0) {
            if (history_count == 0) {
                printf("No commands in history\n");
                free(line);
                continue;
            }
            strcpy(line, history[history_count - 1]);
            printf("%s", line);
        } else {
            add_history(line);
        }

        parse(line, args);
        if (args[0] == NULL) {
            free(line);
            continue;
        }

        if (strcmp(args[0], "exit") == 0) {
            running = 0;
            free(line);
            continue;
        }

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL)
                chdir(getenv("HOME"));
            else if (chdir(args[1]) != 0)
                perror("cd");
            free(line);
            continue;
        }

        if (strcmp(args[0], "pwd") == 0) {
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
            free(line);
            continue;
        }

        if (strcmp(args[0], "help") == 0) {
            printf("Built-in: exit cd pwd help history\n");
            free(line);
            continue;
        }

        if (strcmp(args[0], "history") == 0) {
            for (int i = 0; i < history_count; i++)
                printf("%d %s", i + 1, history[i]);
            free(line);
            continue;
        }

        int bg = is_background(args);
        int pipe_index = has_pipe(args);

        if (pipe_index != -1) {
            args[pipe_index] = NULL;
            char **cmd1 = args;
            char **cmd2 = &args[pipe_index + 1];

            int fd[2];
            pipe(fd);

            if (fork() == 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execvp(cmd1[0], cmd1);
                exit(1);
            }

            if (fork() == 0) {
                dup2(fd[0], STDIN_FILENO);
                close(fd[1]);
                close(fd[0]);
                execvp(cmd2[0], cmd2);
                exit(1);
            }

            close(fd[0]);
            close(fd[1]);
            wait(NULL);
            wait(NULL);
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                execvp(args[0], args);
                printf("Command not found: %s\n", args[0]);
                exit(1);
            } else {
                if (!bg)
                    waitpid(pid, NULL, 0);
            }
        }

        free(line);
    }

    return 0;
}
