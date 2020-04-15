#ifndef JOBS_H
#define JOBS_H

/* includes */
#include <sys/types.h>

/* definitions */
#define JOBS_RUNNING 0
#define JOBS_STOPPED 1
#define JOBS_DONE 2

/* data structures */
struct job
{
    char *arg;
    pid_t pid;
    unsigned int job_id;
    unsigned int job_status : 2;
    int exit_code;
};

struct job_node
{
    struct job data;
    struct job_node *next;
};

/* prototypes */

#endif