// builtins.c
#include "builtins.h"
#include "helpers.h"
#include "path.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
builtin_exit(int argc, char **argv)
{
    (void)argv;
    if (argc != 1) {
        print_error();
        return 1;
    }
    exit(0);
}

int
builtin_cd(int argc, char **argv)
{
    if (argc != 2 || chdir(argv[1]) != 0) {
        print_error();
        return 1;
    }
    return 0;
}

int
builtin_path(int argc, char **argv)
{
    path_clear();
    for (int i = 1; i < argc; i++)
        path_add(argv[i]);
    return 0;
}

static Builtin builtins[] = {
    {"exit", builtin_exit},
    {"cd", builtin_cd},
    {"path", builtin_path},
    {NULL, NULL}
};

Builtin *
builtin_lookup(const char *name)
{
    for (int i = 0; builtins[i].name; i++) {
        if (strcmp(builtins[i].name, name) == 0)
            return &builtins[i];
    }
    return NULL;
}
