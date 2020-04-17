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
};

int smash_jobs(int arg_count, char **arg_vector)
{
    enter("%d, %p", arg_count, arg_vector);
    int retval = EXIT_SUCCESS;

    /* refresh the jobs */
    refresh_jobs();

    /* check arguments */
    if (arg_count > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        retval = EINVAL;
        goto exit;
    }

    /* list all jobs */
    list_jobs();

    /* perform a clean up of DONE/TERMINATED jobs */
    reap_jobs();
exit:
    leave("%d", retval);
    return retval;
}

int smash_fg(int arg_count, char **arg_vector)
{
    enter("%d, %p", arg_count, arg_vector);
    int retval = EXIT_SUCCESS;
    int job_num, job_status;
    struct job_node *j;
    /* check arguments */
    if (arg_count > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg_vector[1])) < 0)
    {
        fprintf(stderr, "%s: %d: invalid job number\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        fprintf(stderr, "%s: %d: no such job\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is not DONE or TERM */
    job_status = j->data.process_status;
    if (job_status == PROCESS_DONE || job_status == PROCESS_TERMINATED)
    {
        fprintf(stderr, "%s: %d: invalid job\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* wake up the job */
    if (job_status == PROCESS_STOPPED)
        kill(j->data.pid, SIGCONT);

    /* send the job to the foreground */
    wait_job(j);
exit:
    leave("%d", retval);
    return retval;
}

int smash_bg(int arg_count, char **arg_vector)
{
    enter("%d, %p", arg_count, arg_vector);
    int retval = EXIT_SUCCESS;
    int job_num, job_status;
    struct job_node *j;
    /* check arguments */
    if (arg_count > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg_vector[1])) < 0)
    {
        fprintf(stderr, "%s: %d: invalid job number\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        fprintf(stderr, "%s: %d: no such job\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* check if the job is suspended */
    job_status = j->data.process_status;
    if (job_status != PROCESS_STOPPED)
    {
        fprintf(stderr, "%s: %d: invalid job\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* wake up the job */
    kill(j->data.pid, SIGCONT);

    /* update the status */
    j->data.process_status = PROCESS_RUNNING;

exit:
    leave("%d", retval);
    return retval;
}

int smash_kill(int arg_count, char **arg_vector)
{
    int retval = EXIT_SUCCESS, job_num, sig;
    struct job_node *j;
    char *nptr;
    /* check arguments */
    if (arg_count > 3)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        retval = EINVAL;
        goto exit;
    }
    /* get job number */
    if ((job_num = atoi(arg_vector[2])) < 0)
    {
        fprintf(stderr, "%s: %d: invalid job number\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* find the job */
    j = get_job((unsigned)job_num);
    if (!j)
    {
        fprintf(stderr, "%s: %d: no such job\n", arg_vector[0], job_num);
        retval = EINVAL;
        goto exit;
    }
    /* prepare the kill call */
    nptr = arg_vector[1];
    if (*nptr != '-')
    {
        fprintf(stderr, "%s: %s: invalid syntax\n", arg_vector[0], arg_vector[1]);
        retval = EINVAL;
        goto exit;
    }
    nptr++;
    /* get the signal number */
    sig = atoi(nptr);
    /* kill */
    if (kill(j->data.pid, sig) < 0)
    {
        fprintf(stderr, "%s: %d: %s\n", arg_vector[0], sig, strerror(errno));
        retval = errno;
        goto exit;
    }

exit:
    return retval;
}

int smash_cd(int arg_count, char **arg_vector)
{
    int retval = EXIT_SUCCESS;
    char *dst = NULL;

    /* check arguments */
    if (arg_count > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        return EINVAL;
    }

    /* get destination */
    dst = arg_vector[1];

    /* if no directory is supplied, go home*/
    if (!dst)
        dst = getenv("HOME");

    /* perform command */
    if (chdir(dst) < 0)
    {
        fprintf(stderr, "%s: %s: %s\n", arg_vector[0], dst, strerror(errno));
        retval = errno;
    }
    return retval;
}

int smash_pwd(int arg_count, char **arg_vector)
{
    int retval = EXIT_SUCCESS;
    char cwd[PATH_MAX];

    /* check arguments */
    if (arg_count > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        return EINVAL;
    }

    /* perform command */
    if (!getcwd(cwd, sizeof(cwd)))
    {
        fprintf(stderr, "%s: %s\n", arg_vector[0], strerror(errno));
        retval = errno;
    }
    else
        printf("%s\n", cwd);

    /* return status */
    leave("%d", retval);
    return retval;
}
