#ifndef JOBS_H
#define JOBS_H

/* includes */
#include "global.h"
#include <sys/types.h>

/* definitions */
#define PROCESS_RUNNING 0
#define PROCESS_STOPPED 1
#define PROCESS_DONE 2

/* data structures */

/* job data */
struct job
{
    struct argument *arg;
    pid_t pid;
    unsigned int process_status : 2;
    int exit_code;
};

/* job node */
struct job_node
{
    unsigned int job_id;
    struct job data;
    struct job_node *next;
    struct job_node *prev;
};

/* prototypes */
int create_job(struct job_node **new_job,
               struct argument *arg,
               pid_t pid,
               unsigned int process_status);
struct job_node *push_job(struct job_node *new_job);
struct job_node *pop_job(unsigned int job_id);
struct job_node *get_job(unsigned int job_id);
int list_jobs(void);

/* declarations */
#define JOB_FMT "[%s]\t%ld\t%ud\t%s\t%d\n"
#endif