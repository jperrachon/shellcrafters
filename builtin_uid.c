// builtin_uid.c
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include "minish.h"

int builtin_uid(int argc, char **argv) {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        printf("User ID: %d, User Name: %s\n", uid, pw->pw_name);
    } else {
        perror("getpwuid");
    }
    return 0;
}
