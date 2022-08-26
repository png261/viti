#include "cursor.h"

#include "buffer.h"
#include "message.h"
#include "util.h"
#include "window.h"

void cursor_refresh(Win *win)
{
    win_scroll(win);
    win_render_numbercol(win);
    win_render_statusline(win);

    int y = win->buf->curline - win->view.yoff;
    int x = win->buf->curcol - win->view.xoff;
    wmove(win->textarea, y, x);
    wrefresh(win->textarea);
}

void cursor_right(Win *win) 
{
    win->buf->curcol++;
    cursor_refresh(win);
}

void cursor_left(Win *win) 
{
    win->buf->curcol--;
    cursor_refresh(win);
}

void cursor_up(Win *win) 
{
    win->buf->curline--;
    cursor_refresh(win);
}

void cursor_down(Win *win) 
{
    win->buf->curline++;
    cursor_refresh(win);
}
