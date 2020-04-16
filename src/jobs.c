#include "jobs.h"

/* head of the jobs list */
static struct job_node *head = NULL;

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

struct job_node *push_job(struct job_node *node)
{
    /* get the head of the list */
    struct job_node **h = &head;

    /* add the new node to the beginning*/
    node->prev = NULL;
    node->next = (*h); //old head

    /* set prev ptr for old head */
    if ((*h))
        (*h)->prev = node;

    /* replace head */
    (*h) = node;

    return node;
}

struct job_node *pop_job(unsigned int job_id)
{
    struct job_node **h = &head, *node;

    /* first, we need to find the job_node */
    node = get_job(job_id);

    /* error checking */
    if (!(*h) || !node)
        return NULL;

    /* node to be popped is head */
    if (*h == node)
        *h = node->next;

    /* next node */
    if (node->next != NULL)
        node->next->prev = node->prev;

    /* prev node */
    if (node->prev != NULL)
        node->prev->next = node->next;

    /* return the popped node */
    return node;
}

struct job_node *get_job(unsigned int job_id)
{
    struct job_node **node = &head;

    while (!(*node))
    {
        if ((*node)->job_id == job_id)
            return *node;
        *node = (*node)->next;
    }

    return NULL;
}

int list_jobs(void)
{
    /* get the head of the list */
    struct job_node **node = &head;

    /* print header */
    fprintf(stdout, "[%s]\t%s\t%s\t%s\t%s\n", "command", "pid", "job #", "status", "exit code");

    /* check if empty */
    if (!(*node))
        return EXIT_SUCCESS;
    /* go to the tail */
    for (; (*node)->next != NULL; *node = (*node)->next)
        ;

    /* print from the tail */
    for (; (*node)->prev != NULL; *node = (*node)->prev)
        fprintf(stdout, "hi");
    return EXIT_SUCCESS;
}