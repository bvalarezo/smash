#include "global.h"
#include "parser.h"

int parseline(const char *prompt, char ***args_buffer)
{
   int retval = EXIT_SUCCESS;
   char *line;
    line = readline(prompt); 
    if(!line)
    {
        retval = -EXIT_FAILURE;
    }


   return retval; 
}
