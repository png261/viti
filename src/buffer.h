#ifndef VITI_BUFFER_H
#define VITI_BUFFER_H

#include "window.h"

typedef struct File {
    char * name;
} File;

typedef struct Line {
    char *content;
    int size;
} Line;

struct buffer {
    Line *lines;
    int nlines;
    File file;
    int curline;
    int curcol;
};

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
#endif 

Buffer * buffer_create();
#endif
