#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "minish.h"

extern int last_status; // Para almacenar el estado del último comando ejecutado
int ejecutar(int argc, char **argv) {
    // Esta función se encarga de ejecutar comandos internos o externos
    struct builtin_struct *builtin_cmd = builtin_lookup(argv[0]);
    if (builtin_cmd) {
        last_status = builtin_cmd->func(argc, argv);
        return last_status;
    } else {
        return externo(argc, argv);
    }
}

int externo(int argc, char **argv) {
    // Esta función se encarga de ejecutar comandos externos utilizando fork y execvp
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
        }
        return last_status;
    } else {
        perror("fork");
        return -1;
    }
}
