#ifndef VITI_BUFFER_H
#define VITI_BUFFER_H

#include "window.h"
#include <stddef.h>

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
typedef struct Line Line;
#endif 

typedef struct File {
    char * name;
} File;

struct Line {
    char *content;
    int size;
    Line *prev;
    Line *next;
};

struct buffer {
    Line *lines;
    int nlines;
    File file;
    int curline;
    int curcol;
};

Buffer * buffer_create();
Line *line_push(Line **line, char *content, size_t size);
Line * new_line(Line *prev, char *content, size_t size);
Line *line_at(Line *line, int at);
void line_insert(Line *prev, char *content, size_t size);
Line *line_insert_after(Line **line, char *content, size_t size);
void line_remove(Line ** head, Line *del);

#endif
