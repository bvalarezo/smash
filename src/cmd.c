#include "debug.h"
#include "jobs.h"
#include "wait.h"
#include "cmd.h"

const struct cmd cmd_list[] = {
    {"jobs", &smash_jobs},
    {"fg", &smash_fg},
    {"bg", &smash_bg},
    {"kill", &smash_kill},
    {"cd", &smash_cd},
    {"pwd", &smash_pwd},
    {"echo", &smash_echo},
};

int smash_jobs(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS;

    /* check arguments */
    if (arg->argc > 1)
    {
        dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }

    /* list all jobs */
    list_jobs(arg->fd_stdout);

    /* perform a clean up of DONE/TERMINATED jobs */
    reap_jobs();
exit:
    leave("%d", retval);
    return retval;
}

int smash_fg(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS;
    int job_num;
    struct job_node *j;
    /* check arguments */
    if (arg->argc != 2)
    {
        if (arg->argc > 2)
            dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        else
            dprintf(arg->fd_stderr, "%s: invalid usage\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg->argv[1])) < 0)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job number\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        dprintf(arg->fd_stderr, "%s: %d: no such job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is not DONE or TERM */
    if (j->data.process_status == PROCESS_DONE || j->data.process_status == PROCESS_TERMINATED)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* wake up the job */
    if (j->data.process_status == PROCESS_STOPPED)
        kill(j->data.pid, SIGCONT);

    /* send the job to the foreground */
    j->data.arg->background = 0;
    wait_job(j, WUNTRACED);
exit:
    leave("%d", retval);
    return retval;
}

int smash_bg(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS;
    int job_num;
    struct job_node *j;
    /* check arguments */
    if (arg->argc != 2)
    {
        if (arg->argc > 2)
            dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        else
            dprintf(arg->fd_stderr, "%s: invalid usage\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg->argv[1])) < 0)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job number\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        dprintf(arg->fd_stderr, "%s: %d: no such job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is in the background*/;
    if (!j->data.arg->background)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is not suspended */
    if (j->data.process_status != PROCESS_STOPPED)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* wake up the job */
    kill(j->data.pid, SIGCONT);

    /* update the status */
    j->data.process_status = PROCESS_RUNNING;
    j->data.arg->background = 1;

exit:
    leave("%d", retval);
    return retval;
}

int smash_kill(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS, job_num, sig;
    struct job_node *j;
    char *nptr;
    /* check arguments */
    if (arg->argc != 3)
    {
        if (arg->argc > 3)
            dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        else
            dprintf(arg->fd_stderr, "%s: invalid usage\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg->argv[2])) < 0)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job number\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        dprintf(arg->fd_stderr, "%s: %d: no such job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is not DONE or TERM */
    if (j->data.process_status == PROCESS_DONE || j->data.process_status == PROCESS_TERMINATED)
    {
        dprintf(arg->fd_stderr, "%s: %d: invalid job\n", arg->argv[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* prepare the kill call */
    nptr = arg->argv[1];
    if (*nptr != '-')
    {
        dprintf(arg->fd_stderr, "%s: %s: invalid syntax\n", arg->argv[0], arg->argv[1]);
        retval = EINVAL;
        goto exit;
    }
    nptr++;
    /* get the signal number */
    sig = atoi(nptr);
    /* kill */
    if (kill(j->data.pid, sig) < 0)
    {
        dprintf(arg->fd_stderr, "%s: %d: %s\n", arg->argv[0], sig, strerror(errno));
        retval = errno;
        goto exit;
    }
    /* wait on the job */
    wait_job(j, WNOHANG | WUNTRACED | WCONTINUED);
exit:
    leave("%d", retval);
    return retval;
}

int smash_cd(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS;
    char *dst = NULL;

    /* check arguments */
    if (arg->argc > 2)
    {
        dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }

    /* get destination */
    dst = arg->argv[1];

    /* if no directory is supplied, go home*/
    if (!dst)
        dst = getenv("HOME");

    /* perform command */
    if (chdir(dst) < 0)
    {
        dprintf(arg->fd_stderr, "%s: %s: %s\n", arg->argv[0], dst, strerror(errno));
        retval = errno;
    }
exit:
    leave("%d", retval);
    return retval;
}

int smash_pwd(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS;
    char cwd[PATH_MAX];

    /* check arguments */
    if (arg->argc > 1)
    {
        dprintf(arg->fd_stderr, "%s: too many arguments\n", arg->argv[0]);
        retval = EINVAL;
        goto exit;
    }

    /* perform command */
    if (!getcwd(cwd, sizeof(cwd)))
    {
        dprintf(arg->fd_stderr, "%s: %s\n", arg->argv[0], strerror(errno));
        retval = errno;
    }
    else
        dprintf(arg->fd_stdout,"%s\n", cwd);
exit:
    leave("%d", retval);
    return retval;
}

int smash_echo(struct argument *arg)
{
    enter("%p", arg);
    int retval = EXIT_SUCCESS, i;
    char *string = NULL;
    /* perform command */
    for (i = 1; i < arg->argc; i++)
    {
        string = arg->argv[i];
        /* check $ sign if this is a enviornment variable */
        if (*string == '$' && *(string + 1) != '\0')
        {
            string++;
            /* get the enviornment variable */
            if ((string = getenv(string)))
                dprintf(arg->fd_stdout, "%s", string);
        }
        else
        {
            dprintf(arg->fd_stdout, "%s", string);
        }
        dprintf(arg->fd_stdout, " ");
    }
    dprintf(arg->fd_stdout, "\n");
    leave("%d", retval);
    return retval;
}