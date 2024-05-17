#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "engine.h"

#define SIZE_NAME   10
#define SIZE_DIR    90
#define SIZE_PNT    SIZE_NAME + SIZE_DIR
#define FNAME_LEN   20
#define DEMIL       ":"
#define VERSION     0.1


char *filename;
const char *usage =
"Usage:\n"
"  wd [option] [jump point]\n"
"Options:\n"
"  -a                         add new directory name point\n"
"  -c                         clear all directory name point\n"
"                             TODO: delete non-existing name points\n"
"  -l                         list directory name point\n"
"  -h                         display this help\n"
"  -r                         remove directory name point\n";


static int wd_error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static int getfname(void)
{
    const char *homepath = getenv("HOME");
    filename = malloc(FNAME_LEN);
    filename = strcat(filename, homepath);
    filename = strcat(filename, "/.warprc");
    return 0;
}

int wd_help(void)
{
    printf("%s", usage);
    return 1;
}

int wd_version(void)
{
    printf("wd (particle) %.1f\n", VERSION);
    return 1;
}

struct bucket *wd_init(void) {
    int count = 0;

    getfname();
    if (access(filename, F_OK) == -1) {
        FILE *f = fopen(filename, "w");
        if (f == NULL)
            wd_error("cannot open file");
    }

    struct bucket *bucket = malloc(sizeof(struct bucket));
    if (bucket == NULL)
        wd_error("allocate bucket memory");

    char *line = malloc(SIZE_PNT);
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        wd_error("can't open file");

    while (fgets(line, SIZE_PNT, f) != NULL) {
        bucket->points[count] = malloc(sizeof(struct point));
        wd_fillpnt(bucket->points[count], line);
        count++;
    }

    bucket->size  = SIZE;
    bucket->avail = count;
    return bucket;
}

void wd_fillpnt(struct point *point, char *line) {
    char *tok;
    tok = strtok(line, DEMIL);
    point->name = strdup(tok);
    tok = strtok(NULL, DEMIL);
    point->dirname  = strdup(tok);
    point->dirname[strlen(point->dirname) -1] = 0;
}

int wd_chkdup(struct bucket *bucket, char *name) {
    size_t avail = bucket->avail;
    for (int i = 0; i < avail; i++) {
        if (strcmp(bucket->points[i]->name, name) == 0)
            return 1;
    }
    return 0;
}

int wd_add(struct bucket *bucket, char *name) {
    char dirname[SIZE_DIR];
    getcwd(dirname, SIZE_DIR);

    if (bucket->avail >= bucket->size)
        wd_error("number of space points is full");

    if (wd_chkdup(bucket, name) != 0) {
        fprintf(stderr, "Warn: point was dublicated '%s'\n", name);
        return 1;
    }

    int i = bucket->avail;
    bucket->points[i] = malloc(sizeof(struct point));
    bucket->points[i]->name = strdup(name);
    bucket->points[i]->dirname  = strdup(dirname);
    bucket->avail++;
    return 0;
}

int wd_free(struct point *point) {
    free(point->name);
    free(point->dirname);
    free(point);
    return 0;
}

int wd_rm(struct bucket *bucket, char *name) {
    int avail = bucket->avail;
    int i;
    for (i = 0; i < avail; i++) {
        if (strcmp(bucket->points[i]->name, name) == 0)
            break;
    }

    if (i == avail) return 1;
    wd_free(bucket->points[i]);
    for ( ; i < (avail - 1); i++)
        bucket->points[i] = bucket->points[i+1];

    bucket->avail--;
    return 0;
}

int wd_save(struct bucket *bucket) {
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "Error2: can't open file '%s'\n", filename);
        return 1;
    }

    for (int i = 0; i < bucket->avail; i++) {
        if (bucket->points[i]->name == NULL)
            continue;
        // TODO: make pretty columns
        fprintf(f, "%s:%s\n", bucket->points[i]->name, bucket->points[i]->dirname);
    }
    return 0;
}

int wd_clean(void) {
    FILE *f = fopen(filename, "w");
    if (f == NULL)
        wd_error("cannot open file");
    return 1;
}

int wd_list(void) {
    FILE *f = fopen(filename, "r");
    char line[SIZE_DIR];
    if (f == NULL) {
        printf("%s\n", filename);
        wd_error("Error: [list] cannot open file");
    }

    while (fgets(line, SIZE_DIR, f))
        printf("%s", line);
    return 1;
}
