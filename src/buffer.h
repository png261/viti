#ifndef VITI_BUFFER_H
#define VITI_BUFFER_H

#include "window.h"
#include <stddef.h>

#ifndef INCLUDE_TYPEDEF_BUFFER
#define INCLUDE_TYPEDEF_BUFFER
    typedef struct buffer Buffer;
    typedef struct Line Line;
#endif 

struct Line {
    char *content;
    int size;
    Line *prev;
    Line *next;
};

struct buffer {
    Line *head;
    Line *tail;
    Line *curline;
    int nlines;
    int iline;
    int icol;
    char * name;
};

Buffer * buffer_create();
void line_push(char *content, size_t size);
void line_remove(Line *del);
void line_insert_after(Line *line, char *content, size_t size);
void line_insert_before(Line *line, char *content, size_t size);

int buffer_progress(Buffer *buf);

#endif // VITI_BUFFER_H
