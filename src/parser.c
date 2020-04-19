#include "debug.h"
#include "parser.h"

int parseline(char *in_line, struct argument **arg, char *delimiters)
{
    enter("%s, %p, %s", in_line, arg, delimiters);
    int retval = EXIT_SUCCESS;
    unsigned int background = 0;
    int fd_in, fd_out, fd_err, argc = 0, i = 0;
    char *line = NULL, *token = NULL, *saveptr = NULL, *char_ptr = NULL;
    char **argv = NULL;
    /* duplicate the readline */
    line = strdup(in_line);
    if (!line)
    {
        retval = -ENOMEM;
        goto fail;
    }

    /* parse the comment out (if any)*/
    for (char_ptr = strchr(line, '#');
         char_ptr != NULL && *char_ptr != '\0';
         char_ptr++)
        *char_ptr = '\0';

    /* check if the whole line was a comment */
    if (!line[0])
    {
        retval = EXIT_FAILURE;
        goto fail;
    }

    /* parse the background character (if any) */
    if ((char_ptr = strchr(line, '&')))
    {
        /* check for syntax error */
        if (strrchr(line, '&') != char_ptr)
        {
            retval = EINVAL;
            goto fail;
        }
        *char_ptr = '\0';
        background++;
    }

    /* parse the redirections */
    if ((retval = parse_redirections(line, &fd_in, &fd_out, &fd_err)) != EXIT_SUCCESS)
        goto fail;

    /* count the number of arguments in the line */
    if ((argc = count_tokens(line, delimiters)) < 0)
    {
        retval = argc;
        goto fail;
    }

    /* syntax error */
    if (argc == 0 && (background ||
                      fd_in != STDIN_FILENO ||
                      fd_out != STDOUT_FILENO ||
                      fd_err != STDERR_FILENO))
    {
        retval = EINVAL;
        goto fail;
    }

    /* There is some arguments */

    /* allocate the arg (vector) */
    argv = (char **)calloc(argc, sizeof(char *));
    if (!argv)
    {
        perror(KRED "Failed to allocate memory" KNRM);
        retval = -ENOMEM;
        goto fail;
    }

    /* parse arguments to buffer */
    token = strtok_r(line, delimiters, &saveptr);
    for (i = 0; i < argc; i++)
    {
        argv[i] = token;
        token = strtok_r(NULL, delimiters, &saveptr);
    }

    /* finally, create the argument struct */
    *arg = (struct argument *)malloc(sizeof(struct argument));
    if (!(*arg))
    {
        perror(KRED "Failed to allocate memory" KNRM);
        retval = -ENOMEM;
        goto fail;
    }
    /* write to struct */
    (*arg)->line = in_line;
    (*arg)->argc = argc;
    (*arg)->argv = argv;
    (*arg)->fd_stdin = fd_in;
    (*arg)->fd_stdout = fd_out;
    (*arg)->fd_stderr = fd_err;
    (*arg)->background = background;

exit:
    leave("%d", retval);
    return retval;
fail:
    if (argv)
        free(argv);
    if (in_line)
        free(in_line);
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

int parse_redirections(char *line, int *fd_in, int *fd_out, int *fd_err)
{
    int retval = EXIT_SUCCESS;
    char *c, *p;
    char buf[FILENAME_MAX];
    size_t len = 0;
    memset(buf, 0, FILENAME_MAX);

    /* assign the default fd */
    *fd_in = STDIN_FILENO;
    *fd_out = STDOUT_FILENO;
    *fd_err = STDERR_FILENO;
    /* parse the entire line */
    for (c = line; *c != '\0'; c++)
    {
        /* stdin */
        if (*c == '<')
        {
            /* skip whitespace */
            for (p = c + 1; *p == ' '; p++)
                ;

            /* read the line to buf for stdin */
            strncpyd(buf, p, FILENAME_MAX, REDIRECTION_DELIM);
            len = strlen(buf);

            /* close previous fd_in (overwrite) */
            if (*fd_in != STDIN_FILENO)
                close(*fd_in);

            /* open the file */
            if ((*fd_in = open(buf, O_RDONLY)) < 0)
            {
                fprintf(stderr, "%s: %s\n", buf, strerror(errno));
                retval = errno;
                goto fail;
            }
            /* clear buffer */
            memset(buf, 0, len);
            /* write to line whitespace */
            memset(c, ' ', len + (p - c));
            /* jump c past the file buffer */
            c += len + (p - c);
        }
        /* stdout */
        else if (*c == '>')
        {
            /* skip whitespace */
            for (p = c + 1; *p == ' '; p++)
                ;

            /* read the line to buf for stdout */
            strncpyd(buf, p, FILENAME_MAX, REDIRECTION_DELIM);
            len = strlen(buf);

            /* close previous *fd_in (overwrite) */
            if (*fd_out != STDOUT_FILENO)
                close(*fd_out);

            /* open the file */
            if ((*fd_out = open(buf, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0)
            {
                fprintf(stderr, "%s: %s\n", buf, strerror(errno));
                retval = errno;
                goto fail;
            }
            /* clear buffer */
            memset(buf, 0, len);
            /* write to line whitespace */
            memset(c, ' ', len + (p - c));
            /* jump c past the file buffer */
            c += len + (p - c);
        }
        /* stderr */
        else if (*c == '2' && *(c + 1) == '>')
        {
            /* skip whitespace */
            for (p = c + 1; *p == ' '; p++)
                ;

            /* read the line to buf for stderr */
            strncpyd(buf, p, FILENAME_MAX, REDIRECTION_DELIM);
            len = strlen(buf);

            /* close previous *fd_in (overwrite) */
            if (*fd_err != STDERR_FILENO)
                close(*fd_err);

            /* open the file */
            if ((*fd_err = open(buf, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0)
            {
                fprintf(stderr, "%s: %s\n", buf, strerror(errno));
                retval = errno;
                goto fail;
            }
            /* clear buffer */
            memset(buf, 0, len);
            /* write to line whitespace */
            memset(c, ' ', len + (p - c));
            /* jump c past the file buffer */
            c += len + (p - c);
        }
    }
exit:
    return retval;
fail:
    if (*fd_in != STDIN_FILENO)
        close(*fd_in);
    if (*fd_out != STDOUT_FILENO)
        close(*fd_out);
    if (*fd_err != STDERR_FILENO)
        close(*fd_err);
    goto exit;
}

char *strncpyd(char *dest, const char *src, size_t n, char *delimiter)
{
    size_t i;

    for (i = 0; i < n && src[i] != '\0' && !chrpbrk(src[i], delimiter); i++)
    {
        if (src[i] != ' ')
            dest[i] = src[i];
    }
    for (; i < n; i++)
        dest[i] = '\0';

    return dest;
}

char *chrpbrk(const char c, const char *accept)
{
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    return strpbrk(buf, accept);
}

void destroy_arg(struct argument *arg)
{
    /* free the arg's assets */
    free(arg->line);
    free(arg->argv);
    /* close the file descriptors */
    if (arg->fd_stdin != STDIN_FILENO)
        close(arg->fd_stdin);
    if (arg->fd_stdout != STDOUT_FILENO)
        close(arg->fd_stdout);
    if (arg->fd_stderr != STDERR_FILENO)
        close(arg->fd_stderr);
    /* free the arg */
    free(arg);
}