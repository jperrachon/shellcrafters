// minish.h
#ifndef MINISH_H
#define MINISH_H

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 100
#define HISTORY_SIZE 100

struct builtin_struct {
    char *cmd;
    int (*func) (int, char **);
    char *help_txt;
};

extern struct builtin_struct builtin_arr[];
extern int status;

int linea2argv(char *linea, int argc, char **argv);
struct builtin_struct *builtin_lookup(char *cmd);
int ejecutar(int argc, char **argv);
int externo(int argc, char **argv);

// Built-in command functions
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

// History functions
void load_history();
void save_history();
void add_to_history(const char *command);

#endif // MINISH_H
