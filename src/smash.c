#include "global.h"
#include "smash.h"
#include "parser.h"
#include "cmd.h"

int smash_init()
{
    int smash_status = STATUS_RUNNING, arg_count = 0;
    char **arg_vector = NULL;
    do
    {
        /* get the arguments from the line */
        arg_count = parseline(PROMPT, &arg_vector, DELIMITERS);

        /* error handle */
        if (arg_count < 0)
        {
            smash_status = (arg_count == -EXIT_FAILURE) ? STATUS_EXIT : arg_count;
            break;
        }

        /* execute command */
        smash_status = smash_execute(arg_count, arg_vector);

        /* free the allocation from readline */
        free(arg_vector[0]);
        /* free the allocation from parseline */
        free(arg_vector);
    } while (smash_status == STATUS_RUNNING);
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
        return STATUS_RUNNING;

    /* check if the argument is a built-in command */
    for (i = 0; i < NUM_CMDS; i++)
    {
        if (0 == strcmp(arg_vector[0], cmd_list[i].func_name))
            return (cmd_list[i].func)(arg_count, arg_vector);
    }

    /* argument is not a built-in */
    return smash_launch(arg_count, arg_vector);
}

int smash_launch(int arg_count, char **arg_vector)
{
    pid_t pid, wpid;
    int retval = STATUS_RUNNING, status;

    pid = fork();
    if (pid == 0)
    {
        // Child process
    }
    else if (pid < 0)
    {
        // Error forking
    }
    else
    {
        // Parent process
    }
    return retval;
}