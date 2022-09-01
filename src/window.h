#ifndef VITI_WINDOW_H
#define VITI_WINDOW_H

#include "buffer.h"

#include <ncurses.h>


#ifndef INCLUDE_TYPEDEF_BUFFER
#define INCLUDE_TYPEDEF_BUFFER
    typedef struct buffer Buffer;
    typedef struct Line Line;
#endif 

#ifndef INCLUDE_TYPEDEF_WINDOW
#define INCLUDE_TYPEDEF_WINDOW
    typedef struct window Win;
#endif 

typedef struct size {
    int lines;
    int cols;
} Size;

struct window {
    Size size;

    Buffer * buf;   // buffer render by window
    Line *tline;    // top line of screen       

    int coloff;     // column offset
    int lineoff;    // line offset

    // textarea
    WINDOW *wtext; 
    int wtext_lines;
    int wtext_cols;

    // status line
    WINDOW *wstatus;
    int wstatus_lines;
    int wstatus_cols;

    // number column
    WINDOW *wnum;
    int wnum_lines;
    int wnum_cols;
};


Win *win_create(Buffer * buf, int lines, int cols, int y, int x);
void win_resize(Win * win, int lines, int cols); 

void win_render(Win * win); 
void win_render_num(Win * win); 
void win_render_lines(Win * win); 
void win_render_status(Win * buf); 

void win_scroll(Win *win);

void update_tline(Win *win); 

#endif // VITI_WINDOW_H
