// builtin_gid.c
#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <stdlib.h>
#include "minish.h"

int builtin_gid(int argc, char **argv) {
    gid_t gid = getgid();
    int ngroups = getgroups(0, NULL);
    gid_t *groups = malloc(ngroups * sizeof(gid_t));

    if (groups == NULL) {
        perror("malloc");
        return 1;
    }

    if (getgroups(ngroups, groups) == -1) {
        perror("getgroups");
        free(groups);
        return 1;
    }

    printf("Primary Group ID: %d\n", gid);
    printf("Supplementary Group IDs: ");
    for (int i = 0; i < ngroups; i++) {
        printf("%d ", groups[i]);
    }
    printf("\n");

    free(groups);
    return 0;
}
