// fileio.c: read file to buffer and save buffer to a file

#define _GNU_SOURCE

#include "fileio.h"

#include "memory.h"
#include "message.h"
#include "util.h"
#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void file_save(char *filename, Buffer *buf) {
    if (filename == NULL) {
        mess_send("No name");
        return;
    }

    FILE *fp = fopen(filename, "w+");

    Line *line = buf->head;
    while (line != NULL) {
        fwrite(line->content, line->size, sizeof(*line->content), fp);
        line = line->next;
        if (line != NULL) { 
            fwrite("\n", 1, 1, fp);
        }
    }

    mess_send("\"%s\" %dL written", buf->name, buf->nlines);
    fclose(fp);
}

void file_open(char *filename, Buffer *buf) {
    FILE *fp = fopen(filename, "r");
    buf->name = filename;

    if (fp == NULL) {
        return;
    }

    char *content = NULL;
    size_t linecap = 0;

    while ((getline(&content, &linecap, fp)) != -1) {
        line_push(content, trim(content));
    }

    free(content);
    fclose(fp);
}
