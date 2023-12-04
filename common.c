
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"

static unsigned int lineNo = 1;

char *stdin_line_read()
{
    char line[500];
    char *ret = fgets(line, sizeof(line), stdin);

    /* check for errors/eof */
    if (ret == NULL) {
        if (feof(stdin)) {
            /* reached end of file */
            return NULL;
        } else {
            /* some other error */
            printf("Error reading line %d\n", lineNo);
            exit(1);
        }
    }

    lineNo++;

    /* check if we got a whole line or not */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
        printf("Error: line %d longer than the buffer (%lu)\n", lineNo, sizeof(line));
        exit(1);
    }

    char *out = strdup(line);
    return out;
}

int max(int a, int b)
{
    if (a > b) return a;
    else return b;
}