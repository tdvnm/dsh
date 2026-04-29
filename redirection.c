// redirection.c
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
detect_redirection(char *cmd, char **outfile)
{
    int count = 0;
    for(char *p = cmd; *p; ++p)
	if(*p == '>')
	    ++count;
    if(count == 0)
	return 0;
    if(count > 1)
	return -1;

    // split on >
    char *pos = strchr(cmd, '>');
    *pos = '\0';
    char *left = cmd;
    char *right = pos + 1;
    trim(left);
    trim(right);

    if(strlen(left) == 0)
	return -1; // must have a command

    // right must be exactly one token
    char copy[1024];
    strncpy(copy, right, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';

    char *save;
    int argc = 0;
    char *tmp = strtok_r(copy, " \t\n", &save);
    while(tmp)
	{
	    argc++;
	    tmp = strtok_r(NULL, " \t\n", &save);
	}
    if(argc != 1)
	return -1;

    *outfile = strdup(right);
    return 1;
}
