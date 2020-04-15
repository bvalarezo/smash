#include "global.h"
#include "jobs.h"

struct job_node *push_job(struct job **head,
                          char *arg,
                          pid_t pid,
                          unsigned int job_status)
{
    struct job_node *new_job;
    /* allocate memory for node */
    if (!(new_job = (struct job_node *)malloc(sizeof(struct job_node))))
        return NULL;
    /* fill in fields */
    new_job->data.pid = pid;
    new_job->data.job_status = job_status;
    /* push the new node to the list */
    return new_job;
}

struct job_node *pop_job(unsigned int job_id)
{
}
struct job_node *get_job(unsigned int job_id)
{
}

int list_jobs(void)
{
    return 0;
}