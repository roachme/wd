#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "engine.h"

/* wd - warp dirnameectory
   It's wrapper for `cd` command which's more suitable if you deal with long and
   friquently used paths.
   */



int main(int argc, char **argv)
{
    int index, c;
    int flags[] = { 0, 0, 0, 0};
    char *(values[]) = { NULL, NULL };
    char *dirname;
    wd_init();

    opterr = 0;

    while ((c = getopt (argc, argv, "a:clhvr:")) != -1) {
        switch (c) {
            case 'c':
                flags[0] = 1;
                break;
            case 'h':
                flags[1] = 1;
                break;
            case 'v':
                flags[2] = 1;
                break;
            case 'l':
                flags[3] = 1;
                break;
            case 'a':
                values[0] = optarg;
                break;
            case 'r':
                values[1] = optarg;
                break;
            case '?':
                if (optopt == 'a' || optopt == 'r')
                    fprintf(stderr, "Option -%c requires argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                            "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    /* Jump into dirnameectory */
    for (index = optind; index < argc; index++) {
        if ((dirname = wd_getdirname(argv[index])) != NULL) {
            printf("%s", dirname);
            wd_deinit();
            return 0;
        }
    }

    /* Optional arguments */
    if (values[0]) {
        if(wd_add(values[0]) == 0)
            wd_save();
    }
    else if (values[1]) {
        if (wd_rm(values[1]) == 0)
            wd_save();
    }

    /* Non-optional arguments */
    if (flags[0])      printf("under development\n"); // wd_clean();
    else if (flags[1]) wd_help();
    else if (flags[2]) wd_version();
    else if (flags[3]) wd_list();

    /*
       Because we won't handle their output as
       dirnameectory path. So return 1 instead of 0.
       */
    wd_deinit();
    return 1;
}
