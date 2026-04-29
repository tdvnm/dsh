// builtins.h
#ifndef BUILTINS_H
#define BUILTINS_H

typedef int (*builtin_fn)(int argc, char **argv);

typedef struct {
    const char *name;
    builtin_fn fn;
} Builtin;

int builtin_exit(int argc, char **argv);
int builtin_cd(int argc, char **argv);
int builtin_path(int argc, char **argv);

Builtin *builtin_lookup(const char *name);

#endif
