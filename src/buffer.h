#ifndef BUFFER_H
#define BUFFER_H
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

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

Buffer * buffer_create();
#endif
