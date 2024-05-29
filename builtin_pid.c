// builtin_pid.c
#include <stdio.h>
#include <unistd.h>
#include "minish.h"

int builtin_pid(int argc, char **argv) {
    printf("Process ID: %d\n", getpid());
    return 0;
}
