#ifndef CMD_H
#define CMD_H

/* includes */

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
int smash_cd(int arg_count, char **arg_vector);
int smash_pwd(int arg_count, char **arg_vector);
int smash_exit(int arg_count, char **arg_vector);

/* declarations */
#define NUM_CMDS 7
//struct cmd exit_cmd = {.func_name = "exit", .func = &smash_exit};
/* global array of commands */
struct cmd cmd_list[NUM_CMDS] = {
    {"jobs", &smash_jobs},
    {"fg", &smash_fg},
    {"bg", &smash_bg},
    {"kill", &smash_kill},
    {"cd", &smash_cd},
    {"pwd", &smash_pwd},
    {"exit", &smash_exit},
};
#endif