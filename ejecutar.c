#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "minish.h"

int ejecutar(int argc, char **argv) {
    struct builtin_struct *builtin_cmd = builtin_lookup(argv[0]);
    if (builtin_cmd) {
        return builtin_cmd->func(argc, argv);
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
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    } else {
        perror("fork");
        return -1;
    }
}
