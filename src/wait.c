#include "debug.h"
#include "wait.h"

int wait_job(struct job_node *j)
{
    enter("%p", j);
    int status;
    pid_t wpid;
    do
    {
        /* wait for any process to reply */
        wpid = waitpid(-1, &status, WUNTRACED);
    } while (!update_job_status(wpid, status) && is_status(j, PROCESS_RUNNING));
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int refresh_jobs(void)
{
    enter("%s", "void");
    int status;
    pid_t wpid;
    do
    {
        /* wait for any process to reply */
        wpid = waitpid(-1, &status, WUNTRACED | WNOHANG);
    } while (!update_job_status(wpid, status));
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int update_job_status(pid_t pid, int status)
{
    enter("%d, %d", pid, status);
    struct job_node *j;

    /* find the job to update */
    j = get_job_by_pid(pid);
    if (!j)
    {
        leave("%d", EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    /* update the status */
    if (WIFSTOPPED(status))
        j->data.process_status = PROCESS_STOPPED;
    else if (WIFEXITED(status))
    {
        j->data.exit_code = WEXITSTATUS(status);
        j->data.process_status = PROCESS_DONE;
    }
    else if (WIFSIGNALED(status))
    {
        j->data.exit_code = WTERMSIG(status);
        j->data.process_status = PROCESS_TERMINATED;
    }
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int is_status(struct job_node *j, unsigned int process_status)
{
    enter("%p, %d", j, process_status);
    int retval = (j->data.process_status == process_status) ? 1 : 0;
    leave("%d", retval);
    return retval;
}