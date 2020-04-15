#include "global.h"
#include "parser.h"

int parseline(const char *prompt, char ***arg_vector, char *delimiters)
{
    int count = 0, i = 0;
    char *line, *token, *saveptr = NULL, *comment_ptr = NULL;
    /* read in the line */
    line = readline(prompt);
    if (!line)
        return -EXIT_FAILURE;

    /* null the comment out (if any)*/
    for (comment_ptr = index(line, '#'); comment_ptr != NULL && *comment_ptr != '\0'; comment_ptr++)
        *comment_ptr = '\0';

    /* check if the whole line was a comment */
    if (!line[0])
        goto fail;

    /* count the number of arguments in the line */
    count = count_tokens(line, delimiters);

    /* error check */
    if (count < 0)
        goto fail;
    if (count == 0)
        goto fail;

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

exit:
    return count;
fail:
    if (line)
        free(line);
    goto exit;
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

int merge_args(char **line, int arg_count, char **arg_vector, char *seperator)
{
    int i, length = 0;
    /* get the length */
    for (i = 0; i < arg_count; i++)
        length += strlen(arg_vector[i]);

    /* allocate the line */
    *line = (char *)malloc(length + (arg_count - 1));
    if (!(*line))
    {
        perror(KRED "Failed to allocate memory" KNRM);
        return -ENOMEM;
    }

    /* merge the arguments */
    for (i = 0; i < (arg_count - 1); i++)
    {
        strcat(*line, arg_vector[i]);
        strcat(*line, seperator);
    }
    strcat(*line, arg_vector[i]);

    return EXIT_SUCCESS;
}