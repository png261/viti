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


void file_save(char *filename, Buffer *buf) 
{
    if (filename == NULL) {
        mess_send("No file name");
        return;
    }
    FILE *fp = fopen(filename, "w+");

    Line * line = buf->lines;
    while(line != NULL){
        fputs(line->content, fp);
        line = line->next;
        if (line != NULL) {
            fputs("\n", fp);
        }
    }

    mess_send("\"%s\" %dL written", buf->file.name, buf->nlines);
    fclose(fp);
}


void file_open(char *filename, Buffer *buf) 
{
    FILE *fp = fopen(filename, "r");
    buf->file.name = filename;

    if (fp == NULL) {
        return;
    }

    char *content = NULL;
    size_t linecap = 0;

    while ((getline(&content, &linecap, fp)) != -1) {
        size_t size = trim(content); 
        line_push(&buf->lines, content, size);
        buf->nlines++;
    }

    free(content);
    fclose(fp);
}
