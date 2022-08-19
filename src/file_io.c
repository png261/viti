#include "file_io.h"

#include "mess.h"
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
    while (*size == '\n' || *size == ' ') {
        *size = '\0';
    }
    return strlen(str);
}

void file_save(Buffer *buf) {
    if (buf->file.name == NULL) {
        mess_send("No file name");
        return;
    }
    FILE *fp = fopen(buf->file.name, "w+");

    for (int y = 0; y < buf->file.lines; y++) {
        Row *row = &buf->rows[y];
        fputs(row->content, fp);
        if (y != buf->file.lines - 1) {
            fputs("\n", fp);
        }
    }

    mess_send("\"%s\" %dL written", buf->file.name, buf->file.lines);

    fclose(fp);
}

void file_open(char *filename, Buffer *buf) {
    FILE *fp = fopen(filename, "r");
    buf->file.name = filename;

    if (!fp) {
        return;
    }

    buf->file.lines = countLines(filename);
    buf->rows = xcalloc(buf->file.lines, sizeof(Row));

    char *line = NULL;
    size_t linecap = 0;
    size_t linelen;

    Row *current = buf->rows;
    while ((getline(&line, &linecap, fp)) != -1) {
        linelen = trim(line);
        current->content = xmalloc(linelen * sizeof(*current->content));
        memcpy(current->content, line, linelen);
        current->size = linelen;
        current++;
    }
    fclose(fp);
}
