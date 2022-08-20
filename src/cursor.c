#include "cursor.h"

#include "buffer.h"
#include "mess.h"
#include "util.h"
#include "window.h"

void cursor_refresh(Win *win)
{
    win_render_numbercol(win);
    win_render_statusline(win);
    wmove(win->textarea, win->buf->line - win->view.yoff,
          win->buf->col - win->view.xoff);
    wrefresh(win->textarea);
}

void cursor_right(Win *win) 
{
    win->buf->col++;
    win_scroll(win);
    cursor_refresh(win);
}

void cursor_left(Win *win) 
{
    win->buf->col--;
    win_scroll(win);
    cursor_refresh(win);
}

void cursor_up(Win *win) 
{
    win->buf->line--;
    win_scroll(win);
    cursor_refresh(win);
}

void cursor_down(Win *win) 
{
    win->buf->line++;
    win_scroll(win);
    cursor_refresh(win);
}
