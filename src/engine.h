#ifndef __ENGINE_UNCLUDED__
#define __ENGINE_UNCLUDED__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#define POINT_SIZE          100
#define POINT_NAME_SIZE     10
#define POINT_DIRNAME_SIZE  100

struct point {
    char name[POINT_NAME_SIZE];
    char dirname[POINT_DIRNAME_SIZE];
    //int mark; /* mark that point is in use */
};

struct bucket {
    size_t size;
    struct point *points[POINT_SIZE];
    size_t avail;
};

/* Public declrations. */
int wd_init();
int wd_add(char *name);
int wd_rm(char *name);
int wd_save();
int wd_list(void);
int wd_clean(void);
int wd_help(void);
char *wd_getdirname(char *name);
int wd_version(void);
int wd_deinit();
#endif
