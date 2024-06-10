#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "minish.h"
#include <time.h>

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

// Busca un comando interno en la lista de comandos internos y devuelve su estructura correspondiente
struct builtin_struct *builtin_lookup(char *cmd) {
    for (int i = 0; builtin_arr[i].cmd != NULL; i++) {
        if (strcmp(builtin_arr[i].cmd, cmd) == 0) {
            return &builtin_arr[i];
        }
    }
    return NULL;
}

// Implementación del comando interno "exit"
// Termina el shell, admite un parámetro que es el status de retorno.
int builtin_exit(int argc, char **argv) {
    int status = 0;
    if (argc > 1) {
        status = atoi(argv[1]);
    }
    save_history();
    exit(status);
}

// Implementación del comando interno "pid"
// Muestra el process id del shell.
int builtin_pid(int argc, char **argv) {
    printf("PID: %d\n", getpid());
    return 0;
}

// Implementación del comando interno "uid"
// Muestra el userid como número y también el nombre de usuario.
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

// Implementación del comando interno "gid"
// Muestra el grupo principal y los grupos secundarios del usuario.
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

// Función para obtener el nombre de usuario a partir del UID
char* get_username(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    } else {
        return "Unknown";
    }
}

// Función para obtener el nombre del grupo a partir del GID
char* get_groupname(gid_t gid) {
    struct group *gr = getgrgid(gid);
    if (gr) {
        return gr->gr_name;
    } else {
        return "Unknown";
    }
}

// Función para obtener la hora de modificación de un archivo
char* get_modified_time(time_t mtime) {
    struct tm *timeinfo;
    timeinfo = localtime(&mtime);
    return asctime(timeinfo);
}

// Función para obtener el tipo de archivo
char *get_file_type(mode_t mode) {
    if (S_ISREG(mode)) return "file";
    if (S_ISDIR(mode)) return "directory";
    if (S_ISCHR(mode)) return "char device";
    if (S_ISBLK(mode)) return "block device";
    if (S_ISFIFO(mode)) return "FIFO";
    if (S_ISLNK(mode)) return "symlink";
    if (S_ISSOCK(mode)) return "socket";
    return "unknown";
}

// Implementación del comando interno "getenv"
// Muestra el valor de una o varias variables de ambiente.
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

// Implementación del comando interno "setenv"
// Define una variable nueva de ambiente o cambia el valor de una variable de ambiente existente.
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

// Implementación del comando interno "unsetenv"
// Elimina una o varias variables de ambiente.
int builtin_unsetenv(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (unsetenv(argv[i]) != 0) {
            perror("unsetenv");
            return 1;
        }
    }
    return 0;
}

// Implementación del comando interno "cd"
// Cambia el directorio corriente.
int builtin_cd(int argc, char **argv) {
    char *dir = (argc > 1) ? argv[1] : getenv("HOME");
    if (chdir(dir) != 0) {
        perror("chdir");
        return 1;
    }
    return 0;
}

// Implementación del comando interno "status"
// Muestra el status de retorno del último comando ejecutado.
int builtin_status(int argc, char **argv) {
    extern int last_status;
    printf("Status: %d\n", last_status);
    return 0;
}

// Implementación del comando interno "help"
// Muestra una ayuda más extensa.
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
// Busca directorios que contengan el texto
void search_directories(const char *root, const char *text) {
    DIR *dp = opendir(root);
    if (dp == NULL) {
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_type == DT_DIR && 
            strcmp(entry->d_name, ".") != 0 && 
            strcmp(entry->d_name, "..") != 0) {
            if (strstr(entry->d_name, text) != NULL) {
                printf("Directory containing '%s': %s/%s\n", text, root, entry->d_name);
            }
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", root, entry->d_name);
            search_directories(path, text);
        }
    }
    closedir(dp);
}

// Implementación del comando interno "dir"
// Simula una ejecución simplificada del comando ls -l.
int builtin_dir(int argc, char **argv) {
    char *dir = (argc > 1) ? argv[1] : ".";
    char *filter = (argc > 2) ? argv[2] : NULL;
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "opendir failed for '%s': %s\n", dir, strerror(errno));
        printf("Searching for directories containing '%s'...\n", dir); // Busca directorios que contengan el texto
        search_directories(".", dir);
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (filter == NULL || strstr(entry->d_name, filter) != NULL) {
            char path[PATH_MAX];
            snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
            struct stat st;
            if (stat(path, &st) == 0) {
                printf("%s ", get_file_type(st.st_mode));
                printf("%ld ", st.st_nlink);
                printf("%s ", get_username(st.st_uid));
                printf("%s ", get_groupname(st.st_gid));
                printf("%lld ", (long long)st.st_size);
                printf("%s ", get_modified_time(st.st_mtime));
                printf("%s\n", entry->d_name);
            }
        }
    }
    closedir(dp);
    return 0;
}
// Implementación del comando interno "history"
// Muestra los N comandos anteriores.
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
