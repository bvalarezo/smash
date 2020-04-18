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
    int (*func)(int, char **);
};

/* prototypes */

/* list all background running jobs */
int smash_jobs(int arg_count, char **arg_vector);
int smash_fg(int arg_count, char **arg_vector);
/* make suspended (stopped) job run in the background.*/
int smash_bg(int arg_count, char **arg_vector);
int smash_kill(int arg_count, char **arg_vector);
/* change the current working directory */
int smash_cd(int arg_count, char **arg_vector);
/* print the current working directory */
int smash_pwd(int arg_count, char **arg_vector);
/* display a line of text */
int smash_echo(int arg_count, char **arg_vector);

/* declarations */
#define NUM_CMDS 7
//struct cmd exit_cmd = {.func_name = "exit", .func = &smash_exit};

/* global array of commands */
extern const struct cmd cmd_list[NUM_CMDS];

#endif