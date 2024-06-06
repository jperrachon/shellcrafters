#ifndef MINISH_H
#define MINISH_H

struct builtin_struct {
    char *cmd;
    int (*func)(int, char **);
    char *help_txt;
};

extern struct builtin_struct builtin_arr[];
extern struct builtin_struct *builtin_lookup(char *cmd);

int linea2argv(char *linea, int argc, char **argv);
int ejecutar(int argc, char **argv);
int externo(int argc, char **argv);

// Funciones para cada comando interno
int builtin_exit(int argc, char **argv);
int builtin_pid(int argc, char **argv);
int builtin_uid(int argc, char **argv);
int builtin_gid(int argc, char **argv);
int builtin_getenv(int argc, char **argv);
int builtin_setenv(int argc, char **argv);
int builtin_unsetenv(int argc, char **argv);
int builtin_cd(int argc, char **argv);
int builtin_status(int argc, char **argv);
int builtin_help(int argc, char **argv);
int builtin_dir(int argc, char **argv);
int builtin_history(int argc, char **argv);

#endif
