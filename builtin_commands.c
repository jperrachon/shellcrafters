#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include "minish.h"

// Definición de los comandos internos
struct builtin_struct builtin_arr[] = {
    {"exit", builtin_exit, "exit [N] - termina el shell, admite un parámetro que es el status de retorno."},
    {"pid", builtin_pid, "pid - muestra el process id del shell."},
    {"uid", builtin_uid, "uid - muestra el userid como número y también el nombre de usuario."},
    {"gid", builtin_gid, "gid - muestra el grupo principal y los grupos secundarios del usuario."},
    {"getenv", builtin_getenv, "getenv variable [variable ...] - muestra el valor de dicha(s) variable(s) de ambiente."},
    {"setenv", builtin_setenv, "setenv variable valor - define una variable nueva de ambiente o cambia el valor de una variable de ambiente existente."},
    {"unsetenv", builtin_unsetenv, "unsetenv var [var ...] - elimina variables de ambiente."},
    {"cd", builtin_cd, "cd [dir] - cambiar el directorio corriente."},
    {"status", builtin_status, "status - muestra el status de retorno del último comando ejecutado."},
    {"help", builtin_help, "help [comando] - muestra una ayuda más extensa."},
    {"dir", builtin_dir, "dir [texto/directorio] - Simula una ejecución simplificada del comando ls -l."},
    {"history", builtin_history, "history [N] - muestra los N comandos anteriores."},
    {NULL, NULL, NULL}
};

struct builtin_struct *builtin_lookup(char *cmd) {
    for (int i = 0; builtin_arr[i].cmd != NULL; i++) {
        if (strcmp(builtin_arr[i].cmd, cmd) == 0) {
            return &builtin_arr[i];
        }
    }
    return NULL;
}

// Implementaciones de los comandos internos
int builtin_exit(int argc, char **argv) {
    int status = 0;
    if (argc > 1) {
        status = atoi(argv[1]);
    }
    exit(status);
}

int builtin_pid(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    return 0;
}

int builtin_uid(int argc, char **argv) {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        printf("UID: %d, Username: %s\n", uid, pw->pw_name);
    } else {
        perror("getpwuid");
    }
    return 0;
}

int builtin_gid(int argc, char **argv) {
    gid_t gid = getgid();
    struct group *gr = getgrgid(gid);
    if (gr) {
        printf("GID: %d, Group: %s\n", gid, gr->gr_name);
    } else {
        perror("getgrgid");
    }
    return 0;
}

int builtin_getenv(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        char *value = getenv(argv[i]);
        if (value) {
            printf("%s=%s\n", argv[i], value);
        } else {
            printf("%s not found\n", argv[i]);
        }
    }
    return 0;
}

int builtin_setenv(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
        return 1;
    }
    if (setenv(argv[1], argv[2], 1) != 0) {
        perror("setenv");
        return 1;
    }
    return 0;
}

int builtin_unsetenv(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (unsetenv(argv[i]) != 0) {
            perror("unsetenv");
            return 1;
        }
    }
    return 0;
}

int builtin_cd(int argc, char **argv) {
    char *dir = (argc > 1) ? argv[1] : getenv("HOME");
    if (chdir(dir) != 0) {
        perror("chdir");
        return 1;
    }
    return 0;
}

int builtin_status(int argc, char **argv) {
    extern int last_status;
    printf("Status: %d\n", last_status);
    return 0;
}

int builtin_help(int argc, char **argv) {
    if (argc == 1) {
        for (int i = 0; builtin_arr[i].cmd != NULL; i++) {
            printf("%s - %s\n", builtin_arr[i].cmd, builtin_arr[i].help_txt);
        }
    } else {
        struct builtin_struct *builtin_cmd = builtin_lookup(argv[1]);
        if (builtin_cmd) {
            printf("%s - %s\n", builtin_cmd->cmd, builtin_cmd->help_txt);
        } else {
            printf("Comando %s no encontrado\n", argv[1]);
        }
    }
    return 0;
}

int builtin_dir(int argc, char **argv) {
    char *dir = (argc > 1) ? argv[1] : ".";
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        perror("opendir");
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(dp);
    return 0;
}

int builtin_history(int argc, char **argv) {
    extern char *history[];
    extern int history_count;
    int limit = (argc > 1) ? atoi(argv[1]) : history_count;
    if (limit > history_count) {
        limit = history_count;
    }
    for (int i = 0; i < limit; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
    return 0;
}
