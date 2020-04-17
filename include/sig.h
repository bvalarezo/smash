#ifndef SIG_H
#define SIG_H

/* includes */
#include "global.h"
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
/* definitions */

/* prototypes */
void sigint_handler(int signal);
void signal_ignore(void);
void signal_restore(void);

/* declarations */

#endif