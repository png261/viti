#ifndef VITI_BUFFER_H
#define VITI_BUFFER_H
#include "window.h"

typedef struct File{
    char * name;
    int lines;
} File;

typedef struct Row {
    char *content;
    int size;
} Row;

struct buffer {
    int line;
    int col;
    Row *rows;
    File file;
};

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
#endif 

Buffer * buffer_create();
#endif
