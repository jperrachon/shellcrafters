#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minish.h"

#define MAX_HISTORY 100

int last_status = 0;  // Para almacenar el estado del último comando ejecutado
char *history[MAX_HISTORY];
int history_count = 0;

void add_to_history(char *linea) {
    if (history_count == MAX_HISTORY) {
        free(history[0]);
        memmove(history, history + 1, (MAX_HISTORY - 1) * sizeof(char *));
        history_count--;
    }
    history[history_count++] = strdup(linea);
}

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

        if (strlen(linea) > 1) {  // Ignorar líneas vacías
            add_to_history(linea);
        }

        argc = linea2argv(linea, 64, argv);
        if (argc > 0) {
            ejecutar(argc, argv);
        }
    }

    free(linea);
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }
    return 0;
}
