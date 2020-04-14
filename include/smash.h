#ifndef SMASH_H
#define SMASH_H

/* includes */
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* defines */
#define PROMPT "smash> "
#define DELIMITERS " \t\r\n"

#define STATUS_RUNNING 1
#define STATUS_EXIT 0
/* prototypes */

int smash_init();
int batch_smash_init(int fd);
int smash_execute(int arg_count, char **arg_vector);
int smash_launch(int arg_count, char **arg_vector);

#endif