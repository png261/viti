#ifndef WINDOW_H
#define WINDOW_H

#include "cursor.h"
#include "buffer.h"
#include <ncurses.h>

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
#endif 

typedef struct{
    int x;
    int y;
} Cursor; 

typedef struct{
    int x;
    int y;
    int xoff;
    int yoff;
}View;

struct window{
    Buffer * buf;
    View view;
    Cursor cur;

    WINDOW *textarea;
    WINDOW *statusline;
    WINDOW *numbercol;
};

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

Win *win_create(Buffer * buf,int height, int width, int y, int x);
void win_render(Win * win); 
void win_render_numbercol(Win * win); 
void win_render_rows(Win * win); 
void win_render_statusline(Win * buf); 

int current_line(Win *win); 
int current_col(Win *win); 
int buffer_progress(Win *win); 

#endif
