#include "debug.h"
#include "wait.h"
#include "sig.h"

void sigchld_handler(int signal)
{
    enter("%d", signal);
    int status = 0;
    pid_t wpid = 0;
    /* wait for any child to reply */
    while ((wpid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (update_job_status(wpid, status))
            break;
    }
    if (d_flag)
    {
        rl_on_new_line();
        rl_replace_line("", 0);
    }
    leave("%s", "void");
}

void sigint_handler(int signal)
{
    fprintf(stdout, "\n");
    enter("%d", signal);
    rl_on_new_line();
    rl_replace_line("", 0);
    leave("%s", "void");
}

int signal_init(void)
{
    int retval = -EXIT_FAILURE;
    enter("%s", "void");
    struct sigaction sigint_action = {
        .sa_handler = &sigint_handler,
        .sa_flags = 0};
    struct sigaction sigchld_action = {
        .sa_handler = &sigchld_handler,
        .sa_flags = 0};
    sigemptyset(&sigint_action.sa_mask);
    sigemptyset(&sigchld_action.sa_mask);
    if (sigaction(SIGINT, &sigint_action, NULL) < 0)
        goto exit;
    if (sigaction(SIGCHLD, &sigchld_action, NULL) < 0)
        goto exit;
    if (signal(SIGQUIT, SIG_IGN) == SIG_ERR)
        goto exit;
    if (signal(SIGTSTP, SIG_IGN) == SIG_ERR)
        goto exit;
    retval = EXIT_SUCCESS;
exit:
    leave("%d", retval);
    return retval;
}

int signal_restore(void)
{
    int retval = -EXIT_FAILURE;
    enter("%s", "void");
    if (signal(SIGINT, SIG_DFL) == SIG_ERR)
        goto exit;
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        goto exit;
    if (signal(SIGTSTP, SIG_DFL) == SIG_ERR)
        goto exit;
    if (signal(SIGCHLD, SIG_DFL) == SIG_ERR)
        goto exit;
    retval = EXIT_SUCCESS;
exit:
    leave("%d", retval);
    return retval;
}