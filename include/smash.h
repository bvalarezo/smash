#ifndef SMASH_H
#define SMASH_H

/* includes */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* definitions */

#define PROMPT "smash> "
#define DELIMITERS " \t\r\n"

#define SMASH_RUNNING 1
#define SMASH_EXIT 0
#define SMASH_ERROR -1

/* prototypes */

int smash_init();
int batch_smash_init(int fd);
int smash_execute(int arg_count, char **arg_vector);
int smash_launch(int arg_count, char **arg_vector);
int smash_launch_builtin(int (*builtin_cmd)(int, char **), int arg_count, char **arg_vector);

/* declarations */

/* Global return value of last command*/
extern int cmd_retval;
#endif