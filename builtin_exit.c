// builtin_exit.c
#include <stdlib.h>
#include "minish.h"

int builtin_exit(int argc, char **argv) {
    if (argc > 1) {
        status = atoi(argv[1]);
    }
    exit(status);
    return 0; // This will never be reached
}
