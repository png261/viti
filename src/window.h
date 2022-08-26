#ifndef VITI_WINDOW_H
#define VITI_WINDOW_H

#include "cursor.h"
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

typedef struct View{
    int xoff;
    int yoff;
} View;

struct window{
    Buffer * buf;
    Line *top_line;
    View view;

    int textarea_lines;
    int textarea_cols;

    WINDOW *textarea;
    WINDOW *statusline;
    WINDOW *numbercol;
};


Win *win_resize(Win * win, const int height, const int width); 
Win *win_create(Buffer * buf, const int height, const int width, const int y, const int x);
void win_render(Win * win); 
void win_render_numbercol(Win * win); 
void win_render_lines(Win * win); 
void win_render_statusline(Win * buf); 

void win_scroll(Win *win);

int buffer_progress(Win *win); 
Line *current_line(Win *win);

#endif
