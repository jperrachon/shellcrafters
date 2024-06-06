#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minish.h"

int main() {
    char *linea = NULL;
    size_t len = 0;
    int argc;
    char *argv[64];  // Máximo número de argumentos

    while (1) {
        printf("(minish) %s:%s > ", getenv("USER"), getenv("PWD"));
        if (getline(&linea, &len, stdin) == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        argc = linea2argv(linea, 64, argv);
        if (argc > 0) {
            ejecutar(argc, argv);
        }
    }

    free(linea);
    return 0;
}
