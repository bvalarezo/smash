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
    {
        retval = -ENOMEM;
        goto exit;
    }
    /* fill in fields */
    (*new_job)->data.arg = arg;
    (*new_job)->data.pid = pid;
    (*new_job)->data.process_status = process_status;

exit:
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
    enter("%p", node);
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

    leave("%p", node);
    return node;
}

struct job_node *pop_job(struct job_node *node)
{
    enter("%p", node);
    struct job_node **h = &head;

    /* error checking */
    if (!(*h) || !node)
    {
        leave("%p", "null");
        return NULL;
    }
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
    leave("%p", node);
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
    enter("%d", pid);
    struct job_node *ptr = head;

    for (; ptr != NULL; ptr = ptr->next)
    {
        if (ptr->data.pid == pid)
        {
            leave("%p", ptr);
            return ptr;
        }
    }
    leave("%s", "null");
    return NULL;
}

int list_jobs(int fd)
{
    enter("%d", fd);
    /* get the head of the list */
    struct job_node *node = head;

    /* print header */
    dprintf(fd, "%s\t%s\t%s\t%s\t%s\n", "job #", "pid", "status", "exit code", "command");

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
            dprintf(fd, JOB_FMT,
                    node->job_id,
                    node->data.pid);
            /* print the status*/
            if (node->data.process_status == PROCESS_RUNNING)
                dprintf(fd, "Running\t");
            else if (node->data.process_status == PROCESS_STOPPED)
                dprintf(fd, "Stopped\t");
            else if (node->data.process_status == PROCESS_DONE)
                dprintf(fd, "Done\t");
            else if (node->data.process_status == PROCESS_TERMINATED)
                dprintf(fd, "Terminated\t");
            /* print the exit code */
            if (node->data.process_status == PROCESS_DONE || node->data.process_status == PROCESS_TERMINATED)
                dprintf(fd, "%d\t", node->data.exit_code);
            else
                dprintf(fd, "\t");
            /* print the command */
            dprintf(fd, "\t%s\n", node->data.arg->line);
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
