#ifndef BUFFER_H
#define BUFFER_H

#include <ncurses.h>
#include "cursor.h"

typedef struct {
    char *content;
    int size;
} Row;

typedef struct{
    int x;
    int y;
    int xoff;
    int yoff;
} View;

typedef struct{
    char * name;
    int lines;
} File;

struct buffer {
    Row *rows;
    View view;
    Cursor cur;
    File file;

    WINDOW *win;
    WINDOW *statusline;
    WINDOW *numbercol;
};
typedef struct buffer Buffer;

Buffer buffer_create(int height, int width, int y, int x);
void buffer_render(Buffer * buf); 
void buffer_render_numbercol(Buffer * buf); 
void buffer_render_rows(Buffer * buf); 
void buffer_render_statusline(Buffer * buf); 

void buffer_scroll(Buffer *buf, int y, int x);
    
#endif
