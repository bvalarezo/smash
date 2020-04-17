#include "debug.h"
#include "sig.h"

void sigint_handler(int signal)
{
    fprintf(stdout, "\n");
    enter("%s", "void");
    rl_on_new_line();
    rl_replace_line("", 0);
    leave("%s", "void");
    rl_redisplay();
}

void signal_ignore(void)
{
    enter("%s", "void");
    struct sigaction sigint_action = {
        .sa_handler = &sigint_handler,
        .sa_flags = 0};
    sigemptyset(&sigint_action.sa_mask);
    sigaction(SIGINT, &sigint_action, NULL);
    // signal(SIGINT, sigint_handler);
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