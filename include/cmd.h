#ifndef CMD_H
#define CMD_H

/* includes */
#include <unistd.h>
#include <limits.h>

/* structs */

/* Command Struct*/
struct cmd
{
    const char *func_name;
    int (*func)(int, char **);
};

/* prototypes */
int smash_jobs(int arg_count, char **arg_vector);
int smash_fg(int arg_count, char **arg_vector);
int smash_bg(int arg_count, char **arg_vector);
int smash_kill(int arg_count, char **arg_vector);
/* change the current working directory */
int smash_cd(int arg_count, char **arg_vector);
/* print the current working directory */
int smash_pwd(int arg_count, char **arg_vector);

/* declarations */
#define NUM_CMDS 6
//struct cmd exit_cmd = {.func_name = "exit", .func = &smash_exit};

/* global array of commands */
extern const struct cmd cmd_list[NUM_CMDS];

#endif