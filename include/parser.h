#ifndef PARSER_H
#define PARSER_H

/* includes */
#include <readline/readline.h>
#include <readline/history.h>

/* data structures */

/* prototypes */

/* 
 * Read a line from the terminal, parse the line, and write the arguments to a vector.
 * 
 * @param prompt 
 * @param argument buffer
 * 
 * @return EXIT_SUCCESS on success
 * @return -EXIT_FAILURE if the input line is NULL and ends with EOF.
 */
int parseline(const char *prompt, char ***args_buffer);

#endif