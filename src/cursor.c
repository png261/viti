// cursor.c: handle move cursor

#include "cursor.h"

#include "buffer.h"
#include "util.h"
#include "window.h"

extern Win *cwin;
extern Buffer *cbuf;

static void cursor_move(Win *win) {
    int y = cbuf->iline - win->lineoff;
    int x = cbuf->icol - win->coloff;
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
    cbuf->icol++;
    cursor_refresh(win);
}

void cursor_left(Win *win) {
    cbuf->icol--;
    cursor_refresh(win);
}

void cursor_up(Win *win) {
    cbuf->iline--;
    if (cbuf->cline != cbuf->head) {
        cbuf->cline = cbuf->cline->prev;
    }
    cursor_refresh(win);
}

void cursor_down(Win *win) {
    cbuf->iline++;
    if (cbuf->cline != cbuf->tail) {
        cbuf->cline = cbuf->cline->next;
    }
    cursor_refresh(win);
}
