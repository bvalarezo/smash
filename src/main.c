#include "global.h"
#include "smash.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int usage(char *name);

/* getopt options */
#define HELP 'h'
#define DEBUG 'd'

/* Usage Message */
#define USAGE "Usage: %s [-dh] [script-file]\n"

static const char *options = "dh";
static int d_flag = 0;

int main(int argc, char *argv[])
{
    char *script_file = NULL;
    int retval = EXIT_SUCCESS, opt, fd;
    while ((opt = getopt(argc, argv, options)) != -1)
    {
        switch (opt)
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
    if (argv[++optind] && script_file)
    {
        fprintf(stderr, KRED "Incorrect argument amount!\nPlease specify no more than one script-file\n" KNRM);
        goto fail;
    }
    if (script_file)
    {
        /* test for valid file open */
        if ((fd = open(script_file, O_RDONLY)) < 0)
        {
            retval = EXIT_FAILURE;
            perror(KRED "Failed to open file\n" KNRM);
            goto exit;
        }
        //non-interactive
        retval = batch_smash_init(fd);
    }
    else
    {
        //interactive
        retval = smash_init();
    }
    goto exit;
fail:
    retval = usage(argv[0]);
exit:
    exit(retval);
}

static int usage(char *name)
{
    fprintf(stderr, USAGE, name);
    return EXIT_FAILURE;
}