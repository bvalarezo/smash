#include "global.h"
#include "smash.h"
#include "parser.h"
#include "cmd.h"

int cmd_retval = EXIT_SUCCESS;

int smash_init()
{
    int smash_status = SMASH_RUNNING, arg_count = 0;
    char **arg_vector = NULL;
    do
    {
        /* get the arguments from the line */
        arg_count = parseline(PROMPT, &arg_vector, DELIMITERS);

        /* error handle */
        if (arg_count < 0)
        {
            smash_status = (arg_count == -EXIT_FAILURE) ? SMASH_EXIT : arg_count;
            break;
        }

        /* execute command */
        smash_status = smash_execute(arg_count, arg_vector);

        /* clean up */
        if (arg_count > 0)
        {
            /* free the allocation from readline */
            free(arg_vector[0]);
            /* free the allocation from parseline */
            free(arg_vector);
        }
    } while (smash_status == SMASH_RUNNING);
    /* SMASH_EXIT on normal exit */
    /* a negative value on a error */
    return smash_status;
}

int batch_smash_init(int fd)
{
    close(fd);
    return 0;
}

int smash_execute(int arg_count, char **arg_vector)
{
    int i;

    /* empty arguments */
    if (arg_count == 0)
        return SMASH_RUNNING;

    /* exit */
    if (0 == strcmp(arg_vector[0], "exit"))
        return SMASH_EXIT;

    /* check if the argument is a built-in command */
    for (i = 0; i < NUM_CMDS; i++)
    {
        if (0 == strcmp(arg_vector[0], cmd_list[i].func_name))
            return smash_launch_builtin((cmd_list[i].func), arg_count, arg_vector);
    }

    /* argument is not a built-in */
    return smash_launch(arg_count, arg_vector);
}

int smash_launch(int arg_count, char **arg_vector)
{
    pid_t pid;
    int retval = SMASH_RUNNING, background = 0, i;
    //char *line, *saveptr = NULL;

    /* check if we have to send the process to the background */
    for (i = 1; i < arg_count; i++)
    {
        /* find the & */
        if (0 == strcmp(arg_vector[i], "&"))
        {
            background++;
            break;
        }
    }

    /* assert proper usage */
    if (arg_vector[i + 1])
    {
        cmd_retval = EINVAL;
        return retval;
    }

    // if (merge_args(&line, arg_count, arg_vector, " ") < 0)
    //     return SMASH_ERROR;

    /* find the & */
    // if (strtok_r(line, "&", &saveptr))
    //     background++;
    // printf("%s\n", line);
    // printf("%s\n", saveptr);

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
        if (execvp(arg_vector[0], arg_vector) < 0)
        {
            if (errno == ENOENT)
                fprintf(stderr, "%s: command not found\n", arg_vector[0]);
            else
                perror(KRED "Failed to exec process" KNRM);
            return SMASH_ERROR;
        }
    }
    else
    {
        /* Parent Process */

        /* Job support */
        if (!background)
            waitpid(pid, NULL, 0);
        else
        {
            /* add the child process to jobs */
        }
    }
    return retval;
}

int smash_launch_builtin(int (*builtin_cmd)(int, char **), int arg_count, char **arg_vector)
{
    int retval = SMASH_RUNNING;

    /* launch the built in command, setting cmd_retval */
    cmd_retval = builtin_cmd(arg_count, arg_vector);

    /* 
    * set the smash retval, based on the command retval 
    * if cmd_retval is negative, this implies a fatal error
    */
    if (cmd_retval < 0)
        retval = SMASH_ERROR;
    return retval;
}