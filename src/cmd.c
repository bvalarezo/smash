#include "global.h"
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
    int retval = EXIT_SUCCESS;

    /* check arguments */
    if (arg_count > 2)
    {
        fprintf(stderr, "%s: too many arguments\n", arg_vector[0]);
        return EINVAL;
    }

    /* list all jobs */
    return retval;
}

int smash_fg(int arg_count, char **arg_vector)
{
    return 1;
}

int smash_bg(int arg_count, char **arg_vector)
{
    return 1;
}
int smash_kill(int arg_count, char **arg_vector)
{
    return 1;
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
    return retval;
}
