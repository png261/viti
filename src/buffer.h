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
void line_push(Line** head_ref, char *content, size_t size);
Line *line_at(Line *line, int at);
void line_remove(Line ** head, Line *del);
void line_add_first(Line ** head, char *content, size_t size);
void line_insert_after(Line *prev, char *content, size_t size);
void line_insert_before(Line ** head_ref, Line * root, char * content, size_t size);

#endif
