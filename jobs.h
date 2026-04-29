// jobs.h - Job control (background processes, fg/bg/jobs)
#ifndef JOBS_H
#define JOBS_H

#include <signal.h>

// Job states
#define JOB_RUNNING 1
#define JOB_STOPPED 2

typedef struct {
    pid_t pid;
    int state;           // JOB_RUNNING or JOB_STOPPED
    char *cmd;           // Command string for display
} Job;

void jobs_init();
void jobs_add(pid_t pid, const char *cmd);
void jobs_update_status(pid_t pid, int state);
void jobs_remove(pid_t pid);
Job *jobs_find(pid_t pid);
void jobs_list();
int jobs_count();
void jobs_cleanup();

#endif
