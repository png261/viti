#include "cursor.h"

#include "buffer.h"
#include "mess.h"
#include "util.h"
#include "window.h"

void cursor_refresh(Win *win) {
    win_render_numbercol(win);
    win_render_statusline(win);
    wmove(win->textarea, win->buf->line - win->view.yoff,
          win->buf->col - win->view.xoff);
    wrefresh(win->textarea);
}

void handleScroll(Win *win) {
    Buffer *buf = win->buf;
    Row *row = current_row(win);

    buf->col = RANGE(buf->col, 0, row->size);
    buf->line = RANGE(buf->line, 0, buf->file.lines - 1);

    if (buf->col < win->view.xoff) {
        win->view.xoff = buf->col;
        win_render_rows(win);
    }
    if (buf->col >= win->view.xoff + win->view.x) {
        win->view.xoff = buf->col - win->view.x + 1;
        win_render_rows(win);
    }

    if (buf->line < win->view.yoff) {
        win->view.yoff = buf->line;
        win_render_rows(win);
    }
    if (buf->line >= win->view.yoff + win->view.y) {
        win->view.yoff = buf->line - win->view.y + 1;
        win_render_rows(win);
    }
}

void cursor_right(Win *win) {
    win->buf->col++;
    handleScroll(win);
    cursor_refresh(win);
}

void cursor_left(Win *win) {
    win->buf->col--;
    handleScroll(win);
    cursor_refresh(win);
}

void cursor_up(Win *win) {
    win->buf->line--;
    handleScroll(win);
    cursor_refresh(win);
}

void cursor_down(Win *win) {
    win->buf->line++;
    handleScroll(win);
    cursor_refresh(win);
}
