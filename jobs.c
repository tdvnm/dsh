// jobs.c - Job control implementation
#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_JOBS 128
static Job job_list[MAX_JOBS];
static int num_jobs = 0;

void
jobs_init()
{
    num_jobs = 0;
    memset(job_list, 0, sizeof(job_list));
}

void
jobs_add(pid_t pid, const char *cmd)
{
    if (num_jobs >= MAX_JOBS)
        return;
    job_list[num_jobs].pid = pid;
    job_list[num_jobs].state = JOB_RUNNING;
    job_list[num_jobs].cmd = strdup(cmd);
    num_jobs++;
}

void
jobs_update_status(pid_t pid, int state)
{
    for (int i = 0; i < num_jobs; i++) {
        if (job_list[i].pid == pid) {
            job_list[i].state = state;
            return;
        }
    }
}

void
jobs_remove(pid_t pid)
{
    for (int i = 0; i < num_jobs; i++) {
        if (job_list[i].pid == pid) {
            free(job_list[i].cmd);
            for (int j = i; j < num_jobs - 1; j++) {
                job_list[j] = job_list[j + 1];
            }
            num_jobs--;
            return;
        }
    }
}

Job *
jobs_find(pid_t pid)
{
    for (int i = 0; i < num_jobs; i++) {
        if (job_list[i].pid == pid)
            return &job_list[i];
    }
    return NULL;
}

void
jobs_list()
{
    if (num_jobs == 0) {
        printf("No jobs\n");
        return;
    }
    for (int i = 0; i < num_jobs; i++) {
        const char *state = (job_list[i].state == JOB_RUNNING) ? "Running" : "Stopped";
        printf("[%d] %d %s   %s\n", i + 1, job_list[i].pid, state, job_list[i].cmd);
    }
}

int
jobs_count()
{
    return num_jobs;
}

void
jobs_cleanup()
{
    for (int i = 0; i < num_jobs; i++)
        free(job_list[i].cmd);
    jobs_init();
}
