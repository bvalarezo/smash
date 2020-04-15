#ifndef PARSER_H
#define PARSER_H

/* includes */
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

/* data structures */

/* prototypes */

/* 
 * Read a line from the terminal, parse the line, and write the arguments to a vector.
 * 
 * 
 * @return the number of arguments
 * @return -EXIT_FAILURE if the input line is NULL and ends with EOF.
 * @return -ENOMEM if a malloc failed
 */
int parseline(const char *prompt, char ***arg_vector, char *delimiters);
/* Count the number of tokens in the line */
int count_tokens(char *line, char *delimiters);
/* merge the argument vector to a single line */
int merge_args(char **line, int arg_count, char **arg_vector, char *seperator);
#endif