#include "parser.h"
#include "debug.h"
#include "cmd.h"
#include "jobs.h"
#include "wait.h"
#include "sig.h"
#include "smash.h"

int cmd_retval = EXIT_SUCCESS;

int smash_init(void)
{
    enter("%s", "void");
    int smash_status = SMASH_RUNNING, ret;
    struct argument *arg = NULL;

    /* we need to ignore ^C and ^Z */
    signal_ignore();
    while (smash_status == SMASH_RUNNING)
    {
        /* get the arguments from the line */
        ret = parseline(KBLU PROMPT KNRM, &arg, DELIMITERS);
        if (ret < 0)
        {
            /* fatal error */
            smash_status = SMASH_ERROR;
            fprintf(stdout, "\n");
            break;
        }
        else if (ret > 0)
        {
            /* parsing error */
            if (ret == EINVAL)
                fprintf(stderr, "syntax error\n");
            continue;
        }

        /* execute command */
        smash_status = smash_execute(arg);
    }
    /* SMASH_EXIT on normal exit */
    /* a negative value on a error */
    leave("%d", smash_status);
    return smash_status;
}

int batch_smash_init(int fd)
{
    close(fd);
    return 0;
}

int smash_execute(struct argument *arg)
{
    enter("%p", arg);
    int i;

    /* empty arguments */
    if (arg->argc == 0)
        return SMASH_RUNNING;

    /* exit */
    if (0 == strcmp(arg->argv[0], "exit"))
        return SMASH_EXIT;

    /* check if the argument is a built-in command */
    for (i = 0; i < NUM_CMDS; i++)
    {
        if (0 == strcmp(arg->argv[0], cmd_list[i].func_name))
            return smash_launch_builtin((cmd_list[i].func), arg);
    }

    /* argument is not a built-in */
    return smash_launch(arg);
}

int smash_launch_builtin(int (*builtin_cmd)(int, char **), struct argument *arg)
{
    enter("%p, %p", builtin_cmd, arg);
    int retval = SMASH_RUNNING;

    /* launch the built in command, setting cmd_retval */
    cmd_retval = builtin_cmd(arg->argc, arg->argv);

    /* 
    * set the smash retval, based on the command retval 
    * if cmd_retval is negative, this implies a fatal error
    */
    if (cmd_retval < 0)
        retval = SMASH_ERROR;
    leave("%d", retval);
    return retval;
}

int smash_launch(struct argument *arg)
{
    enter("%p", arg);
    pid_t pid;
    int retval = SMASH_RUNNING;
    struct job_node *j;
    int infile = arg->fd_stdin, outfile = arg->fd_stdout, errfile = arg->fd_stderr;

    /* fork a child */
    pid = fork();
    if (pid < 0)
    {
        /* error */
        perror(KRED "Failed to fork a process" KNRM);
        return SMASH_ERROR;
    }
    else if (pid == 0)
    {
        /* Child process */
        signal_restore();
        /* perform redirections */

        /* Set the standard input/output channels of the new process.  */
        if (infile != STDIN_FILENO)
        {
            if (dup2(infile, STDIN_FILENO) < 0)
            {
                perror(KRED "Failed to change file descriptors" KNRM);
                exit(EXIT_FAILURE);
            }
            close(infile);
        }
        if (outfile != STDOUT_FILENO)
        {
            if (dup2(outfile, STDOUT_FILENO) < 0)
            {
                perror(KRED "Failed to change file descriptors" KNRM);
                exit(EXIT_FAILURE);
            }
            close(outfile);
        }
        if (errfile != STDERR_FILENO)
        {
            if (dup2(errfile, STDERR_FILENO) < 0)
            {
                perror(KRED "Failed to change file descriptors" KNRM);
                exit(EXIT_FAILURE);
            }
            close(errfile);
        }
        /* check for an error */

        /* execute the new process */
        if (execvp(arg->argv[0], arg->argv) < 0)
        {
            if (errno == ENOENT)
                fprintf(stderr, "%s: command not found\n", arg->argv[0]);
            else
                perror(KRED "Failed to exec process" KNRM);
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        /* Parent Process */

        /* create a new job */
        if (create_job(&j, arg, pid, PROCESS_RUNNING) < 0)
            ;
        /* push the job to the active job list */
        if (!push_job(j))
            ;

        /* Job support */
        if (!arg->background)
        {
            /* foreground */

            /* wait for the job to report back */
            wait_job(j);

            /* job is back */
        }
        else
        {
            /* background */

            /* printf that we have a new job */
            printf("[%d]\t%d\n", j->job_id, j->data.pid);

            /* continue as normal */
        }
    }
    leave("%d", retval);
    return retval;
}
