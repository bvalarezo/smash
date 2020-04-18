#include "debug.h"
#include "parser.h"
#include "jobs.h"

/* head of the jobs list */
static struct job_node *head = NULL;

int create_job(struct job_node **new_job,
               struct argument *arg,
               pid_t pid,
               unsigned int process_status)
{
    enter("%p, %p, %d, %d", new_job, arg, pid, process_status);
    int retval = EXIT_SUCCESS;
    /* allocate memory for node */
    if (!(*new_job = (struct job_node *)calloc(1, sizeof(struct job_node))))
        return -ENOMEM;

    /* fill in fields */
    (*new_job)->data.arg = arg;
    (*new_job)->data.pid = pid;
    (*new_job)->data.process_status = process_status;

    /* push the new node to the list */
    leave("%d", retval);
    return retval;
}

void destroy_job(struct job_node *job_node)
{
    enter("%p", job_node);
    /* free the arg */
    destroy_arg(job_node->data.arg);
    /* free the job */
    free(job_node);
    leave("%s", "void");
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

    /* assign the job id */
    if (node->next)
        node->job_id = node->next->job_id + 1;
    else
        node->job_id = 1;

    return node;
}

struct job_node *pop_job(struct job_node *node)
{
    struct job_node **h = &head;

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
    enter("%d", job_id);
    struct job_node *ptr = head;

    for (; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->job_id == job_id)
        {
            leave("%p", ptr);
            return ptr;
        }
    }
    leave("%s", "null");
    return NULL;
}

struct job_node *get_job_by_pid(pid_t pid)
{
    struct job_node *ptr = head;

    for (; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->data.pid == pid)
            return ptr;
    }
    return NULL;
}

int list_jobs(void)
{
    enter("%s", "void");
    /* get the head of the list */
    struct job_node *node = head;

    /* print header */
    fprintf(stdout, "%s\t%s\t%s\t%s\t%s\n", "job #", "pid", "status", "exit code", "command");

    /* check if empty */
    if (!node)
    {
        leave("%d", EXIT_FAILURE);
        return EXIT_FAILURE;
    }

    /* go to the tail */
    for (; node->next != NULL; node = node->next)
        ;

    /* print from the tail */
    for (; node != NULL; node = node->prev)
    {
        /* print the background job */
        if (node->data.arg->background)
        {
            fprintf(stdout, JOB_FMT,
                    node->job_id,
                    node->data.pid);
            /* print the status*/
            if (node->data.process_status == PROCESS_RUNNING)
                fprintf(stdout, "Running\t");
            else if (node->data.process_status == PROCESS_STOPPED)
                fprintf(stdout, "Stopped\t");
            else if (node->data.process_status == PROCESS_DONE)
                fprintf(stdout, "Done\t");
            else if (node->data.process_status == PROCESS_TERMINATED)
                fprintf(stdout, "Terminated\t");
            /* print the exit code */
            if (node->data.process_status == PROCESS_DONE || node->data.process_status == PROCESS_TERMINATED)
                fprintf(stdout, "%d\t", node->data.exit_code);
            else
                fprintf(stdout, "\t");
            /* print the command */
            fprintf(stdout, "\t%s\n", node->data.arg->line);
        }
    }
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int reap_jobs(void)
{
    enter("%s", "void");
    struct job_node *node = head;

    for (; node != NULL; node = node->next)
    {
        if (node->data.process_status == PROCESS_DONE || node->data.process_status == PROCESS_TERMINATED)
        {
            /* pop this job */
            pop_job(node);
            /* destroy this job */
            destroy_job(node);
        }
    }
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int destroy_all_jobs(void)
{
    enter("%s", "void");
    struct job_node *node = head;

    for (; node != NULL; node = node->next)
    {
        /* pop this job */
        pop_job(node);
        /* destroy this job */
        destroy_job(node);
    }
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
