#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "engine.h"

/*
1. String padding in C
   https://stackoverflow.com/questions/276827/string-padding-in-c
*/

#define SIZE_NAME   10
#define SIZE_DIR    90
#define SIZE_PNT    SIZE_NAME + SIZE_DIR
#define FNAME_LEN   20
#define DELIM       ":"
#define VERSION     0.1
#define CONFIG_FILE_SIZE 100


char configfile[CONFIG_FILE_SIZE];
struct bucket bucket;

static int check_duplicate(char *name) {
    size_t avail = bucket.avail;
    for (int i = 0; i < avail; i++) {
        if (strcmp(bucket.points[i]->name, name) == 0)
            return 1;
    }
    return 0;
}

static int get_longest_name(void)
{
    int max = 0;

    for (int i = 0; i < bucket.avail; ++i) {
        int len = strlen(bucket.points[i]->name);
        if (len > max) {
            max = len;
        }
    }
    return max;
}

static int wd_error(char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static int getfname(void)
{
    const char *homepath = getenv("HOME");
    strcat(configfile, homepath);
    strcat(configfile, "/.warprc");
    return 0;
}

static int get_point(struct point *point, char *line)
{
    char *tok;

    tok = strtok(line, DELIM);
    strncpy(point->name, tok, POINT_NAME_SIZE);

    tok = strtok(NULL, DELIM);
    strncpy(point->dirname, tok, POINT_DIRNAME_SIZE);
    point->dirname[strlen(tok) - 1] = '\0'; // trim newline
    return 1;
}

static int free_point(struct point *point)
{
    free(point);
    return 0;
}

int wd_init(void) {
    int count = 0;
    char line[BUFSIZ];

    getfname();
    if (access(configfile, F_OK) == -1) {
        wd_error("cannot open file");
    }

    FILE *f = fopen(configfile, "r");
    if (f == NULL)
        wd_error("can't open file");

    while (fgets(line, SIZE_PNT, f) != NULL) {
        bucket.points[count] = malloc(sizeof(struct point));
        get_point(bucket.points[count], line);
        count++;
    }

    fclose(f);
    bucket.size  = POINT_SIZE;
    bucket.avail = count;
    return 0;
}

char *wd_getdirname(char *name) {
    int avail = bucket.avail;
    for (int i = 0; i < avail; i++) {
        if (strcmp(bucket.points[i]->name, name) == 0)
            return bucket.points[i]->dirname;
    }
    return NULL;
}

int wd_add(char *name) {
    char dirname[SIZE_DIR];
    int i;


    getcwd(dirname, SIZE_DIR);

    if (strlen(name) > POINT_NAME_SIZE) {
        fprintf(stdout, "wd: too long '%s': point name is too long\n", name);
        return 1;
    }
    if (strlen(dirname) > POINT_DIRNAME_SIZE) {
        fprintf(stdout, "wd: directory path is too deep\n");
        return 1;
    }

    if (bucket.avail >= bucket.size) {
        fprintf(stdout, "wd: number of warp points is full");
        return 1;
    }
    if (check_duplicate(name) != 0) {
        fprintf(stderr, "wd: warp point '%s' already exists.\n", name);
        return 1;
    }

    i = bucket.avail;
    bucket.points[i] = malloc(sizeof(struct point));
    strcpy(bucket.points[i]->name, name);
    strcpy(bucket.points[i]->dirname, dirname);
    bucket.avail++;
    return 0;
}

int wd_deinit() {
    for (int i = 0; i < bucket.avail; ++i) {
        free_point(bucket.points[i]);
    }
    return 0;
}

int wd_rm(char *name) {
    int avail = bucket.avail;
    int i;
    for (i = 0; i < avail; i++) {
        if (strcmp(bucket.points[i]->name, name) == 0)
            break;
    }

    if (i == avail) return 1;
    free_point(bucket.points[i]);
    for ( ; i < (avail - 1); i++)
        bucket.points[i] = bucket.points[i+1];

    bucket.avail--;
    return 0;
}

int wd_save() {
    FILE *f = fopen(configfile, "w");
    if (f == NULL) {
        fprintf(stderr, "Error2: can't open file '%s'\n", configfile);
        return 1;
    }

    for (int i = 0; i < bucket.avail; i++) {
        /*
        if (bucket->points[i]->name == "")
            continue;
        */
        // TODO: make pretty columns
        fprintf(f, "%s:%s\n", bucket.points[i]->name, bucket.points[i]->dirname);
    }
    fclose(f);
    return 0;
}

int wd_clean(void) {
    FILE *f = fopen(configfile, "w");
    if (f == NULL)
        wd_error("cannot open file");
    fclose(f);
    return 1;
}

int wd_list(void) {
    FILE *f = fopen(configfile, "r");
    char line[SIZE_DIR];
    int longest_name = get_longest_name();
    char padding[100] = " ";
    int padlen;
    struct point point;

    if (f == NULL) {
        wd_error("Error: [list] cannot open file");
    }

    while (fgets(line, SIZE_DIR, f)) {
        get_point(&point, line);
        padlen = longest_name - (int)strlen(point.name);
        printf("%*.s%s  ->  %s\n", padlen, padding, point.name, point.dirname);
    }
    fclose(f);
    return 1;
}

int wd_help(void)
{
    printf("%s",
        "Usage:\n"
        "  wd [option] [jump point]\n"
        "Options:\n"
        "  -a                         add new directory name point\n"
        "  -c                         TODO: clean non-existing name points\n"
        "  -l                         list directory name point\n"
        "  -h                         display this help\n"
        "  -r                         remove directory name point\n"
    );
    return 1;
}

int wd_version(void)
{
    printf("wd (particle) %.1f\n", VERSION);
    return 1;
}
