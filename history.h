// history.h - Command history with arrow key support
#ifndef HISTORY_H
#define HISTORY_H

// Initialize history - loads from ~/.kreash_history
void history_init();

// Add a command to history
void history_add(const char *cmd);

// Save history to disk
void history_save();

// Interactive line editor with history navigation
// Reads a line from user with arrow key support
// Returns dynamically allocated string (caller must free)
char *readline_with_history(const char *prompt);

#endif
