#ifndef JOBS_H
#define JOBS_H

/* includes */
#include <sys/types.h>

/* definitions */
#define JOBS_RUNNING 0
#define JOBS_STOPPED 1
#define JOBS_DONE 2

/* data structures */

/* job data */
struct job
{
    char *arg;
    pid_t pid;
    unsigned int job_id;
    unsigned int job_status : 2;
    int exit_code;
};

/* job node */
struct job_node
{
    struct job data;
    struct job_node *next;
};

/* prototypes */
struct job_node *push_job(struct job_node **head, char *arg, pid_t pid, unsigned int job_status);
struct job_node *pop_job(unsigned int job_id);
struct job_node *get_job(unsigned int job_id);
int list_jobs(void);

/* declarations */
/* tree roots */
struct job_node *head = NULL;

#endif