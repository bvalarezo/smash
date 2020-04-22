#include "debug.h"
#include "wait.h"

int wait_job(struct job_node *j, int options)
{
    enter("%p, %d", j, options);
    int status = 0;
    pid_t wpid = 0;
    do
    {
        /* wait for the job to reply */
        wpid = waitpid(j->data.pid, &status, options);

    } while ((!update_job_status(wpid, status) || is_status(j, PROCESS_RUNNING)) && options != WNOHANG);
    leave("%d", EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

int update_job_status(pid_t pid, int status)
{
    enter("%d, %d", pid, status);
    int retval = EXIT_SUCCESS;
    struct job_node *j;
    char code[4];
    memset(code, 0, 4);
    /* find the job to update */
    j = get_job_by_pid(pid);
    if (!j)
    {
        retval = EXIT_FAILURE;
        goto exit;
    }
    /* update the status */
    if (WIFCONTINUED(status))
    {
        j->data.process_status = PROCESS_RUNNING;
    }
    else if (WIFSTOPPED(status))
    {
        j->data.arg->background = 1;
        j->data.process_status = PROCESS_STOPPED;
    }
    else
    {
        /* process terminated */
        if (WIFEXITED(status))
        {
            j->data.exit_code = WEXITSTATUS(status);
            j->data.process_status = PROCESS_DONE;
        }
        else if (WIFSIGNALED(status))
        {
            j->data.exit_code = 128 + WTERMSIG(status);
            j->data.process_status = PROCESS_TERMINATED;
        }
        /* convert the status to a string*/
        snprintf(code, 4, "%d", j->data.exit_code);

        /* set the env */
        debug("$? will be %s", code);
        if (setenv("?", code, 1) < 0)
            retval = EXIT_FAILURE;
        end(j->data.arg->line, j->data.exit_code);
    }
exit:
    leave("%d", retval);
    return retval;
}

int is_status(struct job_node *j, unsigned int process_status)
{
    enter("%p, %d", j, process_status);
    int retval = (j->data.process_status == process_status) ? 1 : 0;
    leave("%d", retval);
    return retval;
}