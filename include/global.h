#ifndef GLOBAL_H
#define GLOBAL_H

/* include */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

/* Colors */
#define KNRM "\033[0m"
#define KRED "\033[1;31m"
#define KGRN "\033[1;32m"
#define KYLW "\033[1;33m"
#define KBWN "\033[0;33m"

/* argument data*/
struct argument
{
    char *line;
    int argc;
    char **argv;
    int fd_stdin, fd_stdout, fd_stderr;
    unsigned int background : 1;
};
#endif
