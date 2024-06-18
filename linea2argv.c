#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
