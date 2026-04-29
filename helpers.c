// helpers.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char error_message[] = "An error has occurred\n";

void
print_error()
{
    ssize_t rc = write(STDERR_FILENO, error_message, strlen(error_message));
    (void)rc; // silence unused-result warning
}

// strip whitespace from both ends
void
trim(char *s)
{
    if(!s)
	return;
    // from the start
    while(*s == ' ' || *s == '\t' || *s == '\n')
	{
	    memmove(s, s + 1, strlen(s));
	}
    // from the end
    size_t l = strlen(s);
    while(l > 0 && (s[l - 1] == ' ' || s[l - 1] == '\t' || s[l - 1] == '\n'))
	{
	    s[--l] = '\0';
	}
}
