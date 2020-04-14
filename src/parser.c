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
    int count = 0, length = strlen(line) + 1;
    char *copy = NULL, *token = NULL, *saveptr = NULL;

    /* allocate a copy */
    copy = (char *)malloc(length * sizeof(char));
    if (!copy)
        return -ENOMEM;

    /* copy from line */
    strncpy(copy, line, length);

    /* count the arguments */
    for (token = strtok_r(copy, delimiters, &saveptr);
         token != NULL;
         token = strtok_r(NULL, delimiters, &saveptr))
        count++;

    /* clean up and return */
    free(copy);
    return count;
}
