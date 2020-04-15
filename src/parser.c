#include "global.h"
#include "parser.h"

int parseline(const char *prompt, char ***arg_vector, char *delimiters)
{
    int count = 0, i = 0;
    char *line, *token, *saveptr = NULL;
    /* read in the line */
    line = readline(prompt);
    if (!line)
        return -EXIT_FAILURE;

    /* count the number of arguments in the line */
    count = count_tokens(line, delimiters);
    if (count < 0)
        return count;
    if (count == 0)
        return 0;

    /* allocate the arg buffer (vector) */
    *arg_vector = (char **)calloc(count, sizeof(char *));
    if (!(*arg_vector))
    {
        perror(KRED "Failed to allocate memory" KNRM);
        free(line);
        return -ENOMEM;
    }

    /* parse arguments to buffer */
    token = strtok_r(line, delimiters, &saveptr);
    for (i = 0; i < count; i++)
    {
        (*arg_vector)[i] = token;
        token = strtok_r(NULL, delimiters, &saveptr);
    }
    return count;
}

int count_tokens(char *line, char *delimiters)
{
    int count = 0;
    char *copy = NULL, *token = NULL, *saveptr = NULL;

    /* create a copy */
    copy = strdup(line);
    if (!copy)
    {
        perror(KRED "Failed to allocate memory" KNRM);
        return -ENOMEM;
    }

    /* count the arguments */
    for (token = strtok_r(copy, delimiters, &saveptr);
         token != NULL;
         token = strtok_r(NULL, delimiters, &saveptr))
        count++;

    /* clean up and return */
    free(copy);
    return count;
}
