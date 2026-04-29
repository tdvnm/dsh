// signals.h - Signal handling (Ctrl+C, Ctrl+Z, etc)
#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

// Initialize signal handlers
// Ctrl+C (SIGINT) won't kill the shell, only affects foreground job
// Ctrl+Z (SIGTSTP) suspends the foreground job, shell continues
void signals_init();

// Set current foreground process (receives signals)
void signals_set_foreground(pid_t pid);

// Clear foreground process
void signals_clear_foreground();

#endif
