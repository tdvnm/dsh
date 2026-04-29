// expansion.h - Handle ~ and $VAR expansion
#ifndef EXPANSION_H
#define EXPANSION_H

// Expands ~ and $VARIABLE in a string
// Returns a new allocated string with expansions applied
// Caller must free the result
char *expand_string(const char *s);

#endif
