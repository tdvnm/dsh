// signals.c - Signal handling implementation
#include "signals.h"
#include "jobs.h"
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static pid_t foreground_pid = 0;

// Handler for SIGINT (Ctrl+C)
// Forwards to foreground process, not the shell
static void
handle_sigint(int sig)
{
    (void)sig;
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
    }
}

// Handler for SIGTSTP (Ctrl+Z)
// Suspends foreground process
static void
handle_sigtstp(int sig)
{
    (void)sig;
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGTSTP);
        jobs_update_status(foreground_pid, JOB_STOPPED);
        printf("\n[Suspended: %d]\n", foreground_pid);
        foreground_pid = 0;
    }
}

// Handler for SIGCHLD
// Detects when background jobs change state
static void
handle_sigchld(int sig)
{
    (void)sig;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        Job *job = jobs_find(pid);
        if (job) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                jobs_remove(pid);
            } else if (WIFSTOPPED(status)) {
                jobs_update_status(pid, JOB_STOPPED);
            }
        }
    }
}

void
signals_init()
{
    struct sigaction sa;

    // Ignore SIGINT and SIGTSTP by default
    // They're forwarded to foreground process by handlers
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigtstp;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTSTP, &sa, NULL);

    // Handle SIGCHLD to clean up background jobs
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);
}

void
signals_set_foreground(pid_t pid)
{
    foreground_pid = pid;
}

void
signals_clear_foreground()
{
    foreground_pid = 0;
}
