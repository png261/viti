#include "fileio.h"

#include "message.h"
#include "window.h"
#include "util.h"
#include "memory.h"

#include <stdlib.h>
#include <string.h>


int countLines(const char *filename) 
{
    FILE *fp = fopen(filename, "r");
    char *line = NULL;
    size_t linecap = 0;
    int count = 0;
    while ((getline(&line, &linecap, fp)) != -1) {
        count++;
    }

    free(line);
    fclose(fp);
    return count;
}

size_t trim(char *str) 
{
    char *c = str + strlen(str) - 1;
    while (*c == '\n' || *c == ' ') {
        *c = '\0';
    }
    return strlen(str);
}

void file_save(const char *filename, Buffer *buf) 
{
    if (filename == NULL) {
        mess_send("No file name");
        return;
    }
    FILE *fp = fopen(filename, "w+");

    for (int y = 0; y < buf->nlines; y++) {
        Line *line = &buf->lines[y];
        fputs(line->content, fp);
        if (y != buf->nlines - 1) {
            fputs("\n", fp);
        }
    }

    mess_send("\"%s\" %dL written", buf->file.name, buf->nlines);

    fclose(fp);
}

void file_open(const char *filename, Buffer *buf) 
{
    FILE *fp = fopen(filename, "r");
    buf->file.name = filename;

    if (fp == NULL) {
        return;
    }

    buf->nlines = countLines(filename);
    buf->lines = xcalloc(buf->nlines, sizeof(Line));

    char *content = NULL;
    size_t linecap = 0;
    size_t linelen;

    Line *current = buf->lines;
    while ((getline(&content, &linecap, fp)) != -1) {
        linelen = trim(content);
        current->content = xmalloc(linelen * sizeof(*current->content));
        memcpy(current->content, content, linelen);
        current->size = linelen;
        current++;
    }

    free(content);
    fclose(fp);
}
