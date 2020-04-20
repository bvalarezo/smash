#include "debug.h"
#include "wait.h"
#include "sig.h"

void sigchld_handler(int signal)
{
    enter("%d", signal);
    int status;
    pid_t wpid;
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

void signal_ignore(void)
{
    enter("%s", "void");
    struct sigaction sigint_action = {
        .sa_handler = &sigint_handler,
        .sa_flags = 0};
    struct sigaction sigchld_action = {
        .sa_handler = &sigchld_handler,
        .sa_flags = 0};
    sigemptyset(&sigint_action.sa_mask);
    sigemptyset(&sigchld_action.sa_mask);
    sigaction(SIGINT, &sigint_action, NULL);
    sigaction(SIGCHLD, &sigchld_action, NULL);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    leave("%s", "void");
}

void signal_restore(void)
{
    enter("%s", "void");
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    leave("%s", "void");
}