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
    char *line;
    /* we need to ignore ^C and ^Z */
    signal_ignore();
    while (smash_status == SMASH_RUNNING)
    {
        /* refresh the jobs */
        //refresh_jobs();

        /* read in the line */
        line = readline(KGRN PROMPT KNRM);
        if (!line)
        {
            smash_status = SMASH_EXIT;
            fprintf(stdout, "exit\n");
            break;
        }
        /* get the arguments from the line */
        ret = parseline(line, &arg, DELIMITERS);
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
    /* clean up */
    destroy_all_jobs();
    /* set the proper return value */
    smash_status = (smash_status == SMASH_ERROR) ? EXIT_FAILURE : EXIT_SUCCESS;
    leave("%d", smash_status);
    return smash_status;
}

int batch_smash_init(FILE *fp)
{
    enter("%p", fp);
    int smash_status = SMASH_RUNNING, ret;
    size_t len;
    struct argument *arg = NULL;
    char *line;
    while (smash_status == SMASH_RUNNING)
    {
        /* refresh the jobs */
        //refresh_jobs();

        /* read in the line */
        if (getline(&line, &len, fp) < 0)
        {
            smash_status = SMASH_EXIT;
            break;
        }
        /* get the arguments from the line */
        ret = parseline(line, &arg, DELIMITERS);
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
    /* clean up */
    destroy_all_jobs();
    /* set the proper return value */
    smash_status = (smash_status == SMASH_ERROR) ? EXIT_FAILURE : EXIT_SUCCESS;
    fclose(fp);
    leave("%d", smash_status);
    return smash_status;
}

int smash_execute(struct argument *arg)
{
    enter("%p", arg);
    execute(arg->line);
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
    int retval = SMASH_RUNNING, cmd_ret = EXIT_SUCCESS;
    int infile = arg->fd_stdin, outfile = arg->fd_stdout, errfile = arg->fd_stderr;
    char code[4];
    memset(code, 0, 4);

    /* set up redirection */
    if (infile != STDIN_FILENO)
    {
        if (dup2(infile, STDIN_FILENO) < 0)
        {
            perror(KRED "Failed to change file descriptors" KNRM);
            retval = SMASH_ERROR;
        }
        close(infile);
    }
    if (outfile != STDOUT_FILENO)
    {
        if (dup2(outfile, STDOUT_FILENO) < 0)
        {
            perror(KRED "Failed to change file descriptors" KNRM);
            retval = SMASH_ERROR;
        }
        close(outfile);
    }
    if (errfile != STDERR_FILENO)
    {
        if (dup2(errfile, STDERR_FILENO) < 0)
        {
            perror(KRED "Failed to change file descriptors" KNRM);
            retval = SMASH_ERROR;
        }
        close(errfile);
    }
    if (retval == SMASH_ERROR)
        goto exit;

    /* launch the built in command, setting cmd_ret */
    cmd_ret = builtin_cmd(arg->argc, arg->argv);
    /* get the retval from the builtin */
    if (cmd_ret != EXIT_SUCCESS)
        cmd_ret = EXIT_FAILURE + 1;

    end(arg->line, cmd_ret);
    /* convert the retval to a string*/
    snprintf(code, 4, "%d", cmd_ret);
    /* set the env */
    debug("$? will be %s", code);
    if (setenv("?", code, 1) < 0)
        retval = SMASH_ERROR;
exit:
    /* free the arg struct */
    destroy_arg(arg);
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
        debug("Entering... execvp(%s, %p)", arg->argv[0], arg->argv);
        if (execvp(arg->argv[0], arg->argv) < 0)
        {
            debug("Leaving... execvp with retval=(%d)", errno);
            if (errno == ENOENT)
            {
                fprintf(stderr, "%s: command not found\n", arg->argv[0]);
                retval = 127;
            }
            else
            {
                perror(KRED "Failed to exec process" KNRM);
                retval = 126;
            }
        }
        exit(retval);
    }
    else
    {
        /* Parent Process */

        /* create a new job */
        if (create_job(&j, arg, pid, PROCESS_RUNNING) < 0)
        {
            perror(KRED "Failed to allocate memory" KNRM);
            retval = SMASH_ERROR;
            goto exit;
        }
        /* push the job to the active job list */
        push_job(j);

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
exit:
    leave("%d", retval);
    return retval;
}
