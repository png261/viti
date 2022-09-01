#ifndef VITI_BUFFER_H
#define VITI_BUFFER_H

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
    Line *head;     // first line 
    Line *tail;     // last line
    Line *cline;    // current line
    int nlines;     // number lines
    int iline;      // current index line
    int icol;       // current index column (index in line content array char)
    char * name;    // buffer name, filename
};

Buffer * buffer_create();
void line_push(char *content, size_t size);
void line_remove(Line *del);
void line_insert_after(Line *line, char *content, size_t size);
void line_insert_before(Line *line, char *content, size_t size);

int buffer_progress(Buffer *buf);

#endif // VITI_BUFFER_H
