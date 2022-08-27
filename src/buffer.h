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
    int iline;
    int icol;
    int nlines;
    char * name;
};

Buffer * buffer_create();
void line_push(Line **head, Line **tail, char *content, size_t size);
Line *line_at(Line *line, int at);
void line_remove(Line **head, Line **tail, Line *del);
void line_insert_after(Line *prev, Line **tail, char *content, size_t size);
void line_insert_before(Line ** head, Line *root, char *content, size_t size);

int buffer_progress(Buffer *buf);
#endif // VITI_BUFFER_H
