#ifndef SMASH_H
#define SMASH_H

/* includes */
#include "global.h"
#include <sys/types.h>

/* definitions */

#define PROMPT "smash> "
#define DELIMITERS " \t\r\n"

#define SMASH_RUNNING 1
#define SMASH_EXIT 0
#define SMASH_ERROR -1

/* prototypes */

int smash_init(void);
int batch_smash_init(int fd);
int smash_execute(struct argument *arg);
int smash_launch(struct argument *arg);
int smash_launch_builtin(int (*builtin_cmd)(int, char **), struct argument *arg);

/* declarations */

/* Global return value of last command*/
extern int cmd_retval;
#endif