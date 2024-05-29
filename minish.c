// minish.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "minish.h"

int status = 0;
char history[HISTORY_SIZE][MAX_COMMAND_LENGTH];
int history_count = 0;

int linea2argv(char *linea, int argc, char **argv) {
    int count = 0;
    char *token = strtok(linea, " \t\n");
    while (token != NULL && count < argc - 1) {
        argv[count++] = strdup(token);
        token = strtok(NULL, " \t\n");
    }
    argv[count] = NULL;
    return count;
}

struct builtin_struct builtin_arr[] = {
    {"exit", builtin_exit, "exit [N] - termina el shell, admite un parámetro que es el status de retorno"},
    {"pid", builtin_pid, "pid - muestra el process id del shell"},
    {"uid", builtin_uid, "uid - muestra el userid como número y también el nombre de usuario"},
    {"gid", builtin_gid, "gid - muestra el grupo principal y los grupos secundarios del usuario"},
    {"getenv", builtin_getenv, "getenv variable [variable ...] - muestra el valor de dicha(s) variable(s) de ambiente"},
    {"setenv", builtin_setenv, "setenv variable valor - define una variable nueva de ambiente o cambia el valor de una variable de ambiente existente"},
    {"unsetenv", builtin_unsetenv, "unsetenv var [var ...] - elimina variables de ambiente"},
    {"cd", builtin_cd, "cd [dir] - cambiar el directorio corriente"},
    {"status", builtin_status, "status - muestra el status de retorno del último comando ejecutado"},
    {"help", builtin_help, "help [comando] - muestra la ayuda de los comandos internos"},
    {"dir", builtin_dir, "dir [texto/directorio] - lista los archivos del directorio"},
    {"history", builtin_history, "history [N] - muestra los N comandos anteriores"}
};

struct builtin_struct *builtin_lookup(char *cmd) {
    for (int i = 0; i < sizeof(builtin_arr) / sizeof(struct builtin_struct); i++) {
        if (strcmp(builtin_arr[i].cmd, cmd) == 0) {
            return &builtin_arr[i];
        }
    }
    return NULL;
}

int ejecutar(int argc, char **argv) {
    struct builtin_struct *builtin = builtin_lookup(argv[0]);
    if (builtin) {
        return builtin->func(argc, argv);
    } else {
        return externo(argc, argv);
    }
}

int externo(int argc, char **argv) {
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
    FILE *file = fopen(strcat(getenv("HOME"), "/.minish_history"), "r");
    if (file != NULL) {
        while (fgets(history[history_count], MAX_COMMAND_LENGTH, file) != NULL && history_count < HISTORY_SIZE) {
            history[history_count][strcspn(history[history_count], "\n")] = '\0';
            history_count++;
        }
        fclose(file);
    }
}

void save_history() {
    FILE *file = fopen(strcat(getenv("HOME"), "/.minish_history"), "w");
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
