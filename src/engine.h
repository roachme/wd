#ifndef __ENGINE_UNCLUDED__
#define __ENGINE_UNCLUDED__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#define SIZE 100

struct point {
    char *name;
    char *dirname;
};

struct bucket {
    size_t size;
    struct point *points[SIZE];
    size_t avail;
};


struct bucket *wd_init();
void wd_fillpnt(struct point *point, char *line);
int wd_chkdup(struct bucket *bucket, char *name);
int wd_add(struct bucket *bucket, char *name);
int wd_free(struct point *point);
int wd_rm(struct bucket *bucket, char *name);
int wd_save(struct bucket *bucket);
int wd_list(void);
int wd_clean(void);
int wd_help(void);
int wd_version(void);
#endif
