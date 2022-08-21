#ifndef VITI_WINDOW_H
#define VITI_WINDOW_H

#include "cursor.h"
#include "buffer.h"

#include <ncurses.h>


#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
    typedef struct buffer Buffer;
    typedef struct Row Row;
#endif 

typedef struct View{
    int x;
    int y;
    int xoff;
    int yoff;
}View;

struct window{
    Buffer * buf;
    View view;

    WINDOW *textarea;
    WINDOW *statusline;
    WINDOW *numbercol;
};

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

Win *win_create(Buffer * buf, const int height, const int width, const int y, const int x);
void win_render(Win * win); 
void win_render_numbercol(Win * win); 
void win_render_rows(Win * win); 
void win_render_statusline(Win * buf); 
void win_render_line(Win * win, int line);

void win_scroll(Win *win);

int current_line(Win *win); 
int current_col(Win *win); 
int buffer_progress(Win *win); 
Row *current_row(Win *win);

#endif
