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
int batch_smash_init(FILE *fp);
int smash_execute(struct argument *arg);
int smash_launch(struct argument *arg);
int smash_launch_builtin(int (*builtin_cmd)(struct argument *), struct argument *arg);

/* declarations */

#endif