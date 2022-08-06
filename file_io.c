#include "file_io.h"
#include <stdlib.h>
#include <string.h>
#include "util.h"

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
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        current->content = malloc(linelen * sizeof(char));
        memcpy(current->content, line, linelen);
        current->size = linelen;
        current++;
    }
    fclose(fp);
}
