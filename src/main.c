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

/* usage Message */
#define USAGE "Usage: %s [-dh] [script-file]\n"

static const char *options = "dh";
int d_flag = 0;

int main(int argc, char *argv[])
{
    char *script_file = NULL;
    int retval = EXIT_SUCCESS, opt;
    FILE *fp = NULL;
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
        if (!(fp = fopen(script_file, "r")))
        {
            retval = EXIT_FAILURE;
            perror(KRED "Failed to open file.\n" KNRM);
            goto exit;
        }
        /* non-interactive mode */
        retval = batch_smash_init(fp);
    }
    else
    {
        /* interactive mode */
        retval = smash_init();
    }
    //TODO: convert retval from smash to appropriate exit_codes
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