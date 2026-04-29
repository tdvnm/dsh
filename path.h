// path.h
#ifndef PATH_H
#define PATH_H

void path_init();
void path_add(const char *p);
int path_remove(const char *p);
void path_clear();
char *find_executable(const char *prog);

#endif
