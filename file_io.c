#include "file_io.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

int countLines(char *filename) {
    FILE *fp = fopen(filename, "r");
    char *line = NULL;
    size_t linecap = 0;
    int count = 0;
    while ((getline(&line, &linecap, fp)) != -1) {
        count++;
    }
    fclose(fp);
    return count;
}

size_t trim(char *str) {
    char *size = str + strlen(str) - 1;
    while (*size == '\n' | *size == ' ') {
        *size = '\0';
    }
    return strlen(str);
}

void file_save(Buffer *buf) {
    FILE *fp = fopen(buf->file.name, "w+");
    if (!fp) {
        die("File not found!");
    }

    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        fputs(row->content, fp);
        fputs("\n", fp);
    }

    fclose(fp);
}

void file_open(char *filename, Buffer *buf) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        die("File not found!");
    }

    buf->file.name = filename;
    buf->file.lines = countLines(filename);
    buf->rows = calloc(buf->file.lines, sizeof(Row));

    char *line = NULL;
    size_t linecap = 0;
    size_t linelen;

    Row *current = buf->rows;
    while ((getline(&line, &linecap, fp)) != -1) {
        linelen = trim(line);
        current->content = malloc(linelen * sizeof(char));
        memcpy(current->content, line, linelen);
        current->size = linelen;
        current++;
    }
    fclose(fp);
}
