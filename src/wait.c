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
    if (WIFSTOPPED(status))
    {
        j->data.arg->background = 1;
        j->data.process_status = PROCESS_STOPPED;
    }
    else if (WIFEXITED(status))
    {
        j->data.exit_code = WEXITSTATUS(status);
        j->data.process_status = PROCESS_DONE;

        /* convert the status to a string*/
        sprintf(code, "%d", status);

        /* set the env */
        if (setenv("?", code, 1) < 0)
            retval = EXIT_FAILURE;
    }
    else if (WIFSIGNALED(status))
    {
        j->data.exit_code = WTERMSIG(status);
        j->data.process_status = PROCESS_TERMINATED;
        /* convert the retval to a string*/
        sprintf(code, "%d", status);

        /* set the env */
        if (setenv("?", code, 1) < 0)
            retval = EXIT_FAILURE;
    }
exit:
    leave("%d", EXIT_SUCCESS);
    return retval;
}

int is_status(struct job_node *j, unsigned int process_status)
{
    enter("%p, %d", j, process_status);
    int retval = (j->data.process_status == process_status) ? 1 : 0;
    leave("%d", retval);
    return retval;
}