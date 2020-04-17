#ifndef DEBUG_H
#define DEBUG_H

/* includes */
#include "global.h"

extern int d_flag;

/* Macro function that, on enter, prints function name and args based on the DBGVAL*/
#define enter(fmt, ...)                                                                        \
    do                                                                                         \
    {                                                                                          \
        if (d_flag)                                                                            \
            fprintf(stderr, KBWN "Entering... %s(" fmt ")\n" KNRM, __FUNCTION__, __VA_ARGS__); \
    } while (0)

/* Macro function that, on leave, prints function name and retval based on the DBGVAL*/
#define leave(fmt, retval)                                                                            \
    do                                                                                                \
    {                                                                                                 \
        if (d_flag)                                                                                   \
            fprintf(stderr, KBWN "Leaving... %s with retval=(" fmt ")\n" KNRM, __FUNCTION__, retval); \
    } while (0)

#endif