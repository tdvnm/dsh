// path.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATHS 64
char *paths[MAX_PATHS];
int npaths = 0;

void
path_init()
{
    paths[0] = strdup("/bin");
    npaths = 1;
}

void
path_add(const char *p)
{
    if(npaths >= MAX_PATHS)
	return;
    for(int i = npaths; i > 0; --i)
	paths[i] = paths[i - 1];
    paths[0] = strdup(p);
    npaths++;
}

int
path_remove(const char *p)
{
    for(int i = 0; i < npaths; i++)
	{
	    if(strcmp(paths[i], p) == 0)
		{
		    free(paths[i]);
		    for(int j = i; j < npaths - 1; j++)
			paths[j] = paths[j + 1];
		    npaths--;
		    return 0;
		}
	}
    return -1;
}

void
path_clear()
{
    for(int i = 0; i < npaths; i++)
	free(paths[i]);
    npaths = 0;
}

char *
find_executable(const char *prog)
{
    if(npaths == 0)
	return NULL;
    for(int i = 0; i < npaths; i++)
	{
	    char candidate[1024];
	    snprintf(candidate, sizeof(candidate), "%s/%s", paths[i], prog);
	    if(access(candidate, X_OK) == 0)
		return strdup(candidate);
	}
    return NULL;
}
