#include "cursor.h"

#include "buffer.h"
#include "message.h"
#include "util.h"
#include "window.h"

extern Win * curwin;
extern Buffer * curbuf;


static void cursor_move(Win *win){
    int y = curbuf->iline - win->yoff;
    int x = curbuf->icol - win->xoff;
    wmove(win->wtext, y, x);
    win_render_numbercol(win);
    win_render_statusline(win);
}


void cursor_refresh(Win *win)
{
    cursor_move(win);
    wrefresh(win->wtext);
    win_scroll(win);
}


void cursor_right(Win *win) 
{
    curbuf->icol++;
    Line *line = curbuf->curline;
    LIMIT(curbuf->icol, 0, line == NULL ? 0 : line->size);
    cursor_refresh(win);
}


void cursor_left(Win *win) 
{
    curbuf->icol--;
    Line *line = curbuf->curline;
    LIMIT(curbuf->icol, 0, line == NULL ? 0 : line->size);
    cursor_refresh(win);
}


void cursor_up(Win *win) 
{
    curbuf->iline--;
    LIMIT(curbuf->iline, 0, curbuf->nlines - 1);
    if(curbuf->curline->prev != NULL) {
        curbuf->curline = curbuf->curline->prev; 
    }
    cursor_refresh(win);
}


void cursor_down(Win *win) 
{
    curbuf->iline++;
    if(curbuf->curline->next != NULL) {
        curbuf->curline = curbuf->curline->next; 
    }
    LIMIT(curbuf->iline, 0, curbuf->nlines - 1);
    cursor_refresh(win);
}
