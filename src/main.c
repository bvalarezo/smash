#include <getopt.h>
#include "global.h"
#include "smash.h"

static int usage(char *name);

/* getopt options */
#define HELP 'h'
#define DEBUG 'd'


/* Usage Message */
#define USAGE "Usage: %s [-dh] [script-file]\n"

static const char *options = "dh";
static int d_flag = 0;

int main(int argc, char*argv[])
{
    char *script_file = NULL;
    int retval = EXIT_SUCCESS, opt;
    while((opt=getopt(argc, argv, options)) != -1)
    {
        switch(opt)
        {
            case HELP:
                goto fail;
                break;
            case DEBUG:
                d_flag++;
                break;
            default:
                goto fail;
                break;
        }
    }
    script_file = argv[optind];
    if(argv[++optind])
    {
        fprintf(stderr, KRED "Incorrect argument amount!\nPlease specify no more than one script-file" KNRM);
        goto fail;
    }
    if(script_file)
    {
        //non-interactive
        //test file
    }
    else
    {
        //interactive
    }
    goto exit;
fail:
    retval = usage(argv[0]);
exit:
    return retval;
}

static int usage(char *name)
{
    fprintf(stderr, USAGE ,name);
    return EXIT_FAILURE;
}