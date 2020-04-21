#ifndef SIG_H
#define SIG_H

/* includes */
#include "global.h"
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
/* definitions */

/* prototypes */
void sigchld_handler(int signal);
void sigint_handler(int signal);
int signal_init(void);
int signal_restore(void);

/* declarations */

#endif