#include "jobs.h"

/* head of the list */
struct job_node *head = NULL;

int create_job(struct job_node **new_job,
               struct argument *arg,
               pid_t pid,
               unsigned int process_status)
{
    int retval = EXIT_SUCCESS;
    /* allocate memory for node */
    if (!(*new_job = (struct job_node *)calloc(1, sizeof(struct job_node))))
        return -ENOMEM;

    /* fill in fields */
    (*new_job)->data.arg = arg;
    (*new_job)->data.pid = pid;
    (*new_job)->data.process_status = process_status;

    /* push the new node to the list */
    return retval;
}

struct job_node *pop_job(unsigned int job_id)
{
    return NULL;
}
struct job_node *get_job(unsigned int job_id)
{
    return NULL;
}

int list_jobs(void)
{
    return 0;
}