#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 100
#define HISTORY_SIZE 100

int status = 0;
char history[HISTORY_SIZE][MAX_COMMAND_LENGTH];
int history_count = 0;

int linea2argv(char *linea, int argc, char **argv) {
    int count = 0;
    char *token = strtok(linea, " \t\n");
    while (token != NULL && count < argc - 1) {
        argv[count++] = token;
        token = strtok(NULL, " \t\n");
    }
    argv[count] = NULL;
    return count;
}

int builtin_exit(int argc, char **argv) {
    if (argc > 1) {
        status = atoi(argv[1]);
    }
    exit(status);
    return 0; // This will never be reached
}

int builtin_pid(int argc, char **argv) {
    printf("Process ID: %d\n", getpid());
    return 0;
}

int builtin_uid(int argc, char **argv) {
    printf("User ID: %d\n", getuid());
    return 0;
}

int builtin_gid(int argc, char **argv) {
    printf("Group ID: %d\n", getgid());
    return 0;
}

int ejecutar(int argc, char **argv) {
    if (strcmp(argv[0], "exit") == 0) return builtin_exit(argc, argv);
    if (strcmp(argv[0], "pid") == 0) return builtin_pid(argc, argv);
    if (strcmp(argv[0], "uid") == 0) return builtin_uid(argc, argv);
    if (strcmp(argv[0], "gid") == 0) return builtin_gid(argc, argv);

    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        return WIFEXITED(wstatus) ? WEXITSTATUS(wstatus) : -1;
    } else {
        perror("fork");
        return -1;
    }
}

void prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("(minish) %s:%s > ", getenv("USER"), cwd);
    } else {
        perror("getcwd");
    }
}

void load_history() {
    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/.minish_history", getenv("HOME"));
    FILE *file = fopen(history_path, "r");
    if (file != NULL) {
        while (fgets(history[history_count], MAX_COMMAND_LENGTH, file) != NULL && history_count < HISTORY_SIZE) {
            history[history_count][strcspn(history[history_count], "\n")] = '\0';
            history_count++;
        }
        fclose(file);
    }
}

void save_history() {
    char history_path[1024];
    snprintf(history_path, sizeof(history_path), "%s/.minish_history", getenv("HOME"));
    FILE *file = fopen(history_path, "w");
    if (file != NULL) {
        for (int i = 0; i < history_count; i++) {
            fprintf(file, "%s\n", history[i]);
        }
        fclose(file);
    }
}

void add_to_history(const char *command) {
    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count++], command);
    } else {
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], command);
    }
}

int main() {
    load_history();
    char line[MAX_COMMAND_LENGTH];
    char *argv[MAX_ARGUMENTS];
    int argc;

    while (1) {
        prompt();
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        add_to_history(line);
        argc = linea2argv(line, MAX_ARGUMENTS, argv);
        if (argc == 0) {
            continue;
        }

        status = ejecutar(argc, argv);
    }

    save_history();
    return status;
}
