#ifndef PARSER_H
#define PARSER_H

/* includes */
#include "global.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* data structures */

/* prototypes */

/* 
 * Read a line from the terminal, parse the line, and write the arguments to a vector.
 * 
 * 
 * @return EXIT_SUCCESS or negative error code
 */
int parseline(const char *prompt, struct argument **arg, char *delimiters);
/* Count the number of tokens in the line */
int count_tokens(char *line, char *delimiters);
/* merge the argument vector to a single line */
int merge_args(char **line, int arg_count, char **arg_vector, char *seperator);

/* 
 * Checks if the '&' symbol is present. 
 * @return EXIT_SUCCESS or -EXIT_FAILURE
 */
int check_background_argument(int arg_count, char **arg_vector, unsigned int *background);

/* 
 * Parse for redirection and open proper file descriptors
 *  
 * @return EXIT_SUCCESS or -EXIT_FAILURE
 */
int parse_redirections(char *line, int *fd_in, int *fd_out, int *fd_err);

/* Copy no more than N characters of SRC to DEST or until you hit a character in the delimiter */
char *strncpyd(char *dest, const char *src, size_t n, char *delimiter);

char *chrpbrk(const char c, const char *accept);
/* macros */
#define REDIRECTION_DELIM " ><"

#endif