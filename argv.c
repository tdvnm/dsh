// argv.c
#include <stdlib.h>
#include <string.h>

char **
build_argv(char *cmd, int *argc_out)
{
    char *copy = strdup(cmd);
    char *save;
    int cap = 8, n = 0;
    char **argv = malloc(cap * sizeof(char *));
    char *tok = strtok_r(copy, " \t\n", &save);
    while(tok)
	{
	    if(n >= cap)
		{
		    cap *= 2;
		    argv = realloc(argv, cap * sizeof(char *));
		}
	    argv[n++] = strdup(tok);
	    tok = strtok_r(NULL, " \t\n", &save);
	}
    argv[n] = NULL;
    *argc_out = n;
    free(copy);
    return argv;
}
