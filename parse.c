// parse.c
#include "helpers.h"
#include <stdlib.h>
#include <string.h>

char **
split_char(const char *s, char delim, int *out_count)
{
    char *dup = strdup(s);
    char *p = dup;
    char *tok;
    int cap = 8, n = 0;
    char **arr = malloc(cap * sizeof(char *));
    while((tok = strsep(&p, (char[]){ delim, '\0' })) != NULL)
	{
	    char *t = strdup(tok);
	    trim(t);
	    if(n >= cap)
		{
		    cap *= 2;
		    arr = realloc(arr, cap * sizeof(char *));
		}
	    arr[n++] = t;
	}
    free(dup);
    *out_count = n;
    return arr;
}
