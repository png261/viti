// cursor.c: handle move cursor

#include "cursor.h"

#include "buffer.h"
#include "util.h"
#include "window.h"

extern Win *curwin;
extern Buffer *curbuf;

static void cursor_move(Win *win) {
    int y = curbuf->iline - win->yoff;
    int x = curbuf->icol - win->xoff;
    wmove(win->wtext, y, x);
}

void cursor_refresh(Win *win) {
    win_scroll(win);
    cursor_move(win);
    win_render_num(win);
    win_render_status(win);
    wrefresh(win->wtext);
}

void cursor_right(Win *win) {
    curbuf->icol++;
    cursor_refresh(win);
}

void cursor_left(Win *win) {
    curbuf->icol--;
    cursor_refresh(win);
}

void cursor_up(Win *win) {
    curbuf->iline--;
    if (curbuf->curline != curbuf->head) {
        curbuf->curline = curbuf->curline->prev;
    }
    cursor_refresh(win);
}

void cursor_down(Win *win) {
    curbuf->iline++;
    if (curbuf->curline != curbuf->tail) {
        curbuf->curline = curbuf->curline->next;
    }
    cursor_refresh(win);
}
