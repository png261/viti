#include "cursor.h"

#include "buffer.h"
#include "message.h"
#include "util.h"
#include "window.h"

extern Win * curwin;
extern Buffer * curbuf;


static void cursor_move(Win *win){
    int y = win->buf->curline - win->view.yoff;
    int x = win->buf->curcol - win->view.xoff;
    Line *line = curbuf->current_line;
    LIMIT(x, 0, line == NULL ? 0 : line->size);
    LIMIT(y, 0, curbuf->nlines - 1);
    wmove(win->textarea, y, x);
}


void cursor_refresh(Win *win)
{
    cursor_move(win);
    wrefresh(win->textarea);
    win_scroll(win);
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
    curbuf->current_line = curbuf->current_line->prev; 
    win_render_numbercol(win);
    win_render_statusline(win);
    cursor_refresh(win);
}


void cursor_down(Win *win) 
{
    win->buf->curline++;
    curbuf->current_line = curbuf->current_line->next; 
    win_render_numbercol(win);
    win_render_statusline(win);
    cursor_refresh(win);
}
