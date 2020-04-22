#ifndef CMD_H
#define CMD_H

/* includes */
#include "global.h"
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>

/* structs */

/* Command Struct*/
struct cmd
{
    const char *func_name;
    int (*func)(struct argument *);
};

/* prototypes */

/* list all background running jobs */
int smash_jobs(struct argument *arg);
int smash_fg(struct argument *arg);
/* make suspended (stopped) job run in the background.*/
int smash_bg(struct argument *arg);
int smash_kill(struct argument *arg);
/* change the current working directory */
int smash_cd(struct argument *arg);
/* print the current working directory */
int smash_pwd(struct argument *arg);
/* display a line of text */
int smash_echo(struct argument *arg);

/* declarations */
#define NUM_CMDS 7
//struct cmd exit_cmd = {.func_name = "exit", .func = &smash_exit};

/* global array of commands */
extern const struct cmd cmd_list[NUM_CMDS];

#endif