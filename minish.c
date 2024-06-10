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
    linea[strcspn(linea, "\n")] = '\0';
    history[history_count++] = strdup(linea);
}

void load_history() {
    printf("\nloading history...\n");
    char *home_dir = getenv("HOME");
    char history_file[256];
    snprintf(history_file, sizeof(history_file), "%s/.minish_history", home_dir);
    
    FILE *file;
    file = fopen(history_file, "r");
    if (file==NULL){
        printf("\nno minish_history found, creating file...\n");
        file = fopen(history_file, "w");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';  // Remove trailing newline character
        add_to_history(line);
    }

    fclose(file);
}

void save_history() {
    printf("\nsaving history...\n");
    char *home_dir = getenv("HOME");
    char history_file[256];
    snprintf(history_file, sizeof(history_file), "%s/.minish_history", home_dir);

    FILE *file = fopen(history_file, "w");
    if (file == NULL) {
        printf("no se pudo guardar el historial de comandos");
        perror("fopen");
        return;
    }

    for (int i = 0; i < history_count; i++) {
        fprintf(file, "%s\n", history[i]);
    }

    fclose(file);
}

int main() {
    char *linea = NULL;
    size_t len = 0;
    int argc;
    char *argv[64];  // Máximo número de argumentos

    load_history();

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