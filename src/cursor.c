#include "buffer.h"
#include "cursor.h"
#include "mess.h"
#include "util.h"
#include "window.h"

void cursor_refresh(Win *win) {
    win_render_numbercol(win);
    win_render_statusline(win);
    wmove(win->textarea, win->cur.y, win->cur.x);
    wrefresh(win->textarea);
}

void cursor_check(Win *win) {
    Buffer *buf = win->buf;
    Row *row = current_row(win);
    int maxX = MIN(row->size < 0 ? 0 : row->size, win->view.x - 1);
    int maxY = MIN(buf->file.lines, win->view.y) - 1;

    win->cur.x = RANGE(win->cur.x, 0, maxX);
    win->cur.y = RANGE(win->cur.y, 0, maxY);
}

void handleScroll(Win *win) {
    Buffer *buf = win->buf;
    Row *row = current_row(win);
    if (win->cur.x < 0) {
        win->view.xoff += win->cur.x;
    }

    if (win->cur.x >= win->view.x) {
        win->view.xoff += win->cur.x + 1 - win->view.x;
    }

    if (win->cur.y < 0) {
        win->view.yoff += win->cur.y;
    }

    if (win->cur.y >= win->view.y) {
        win->view.yoff += win->cur.y + 1 - win->view.y;
    }

    int maxXoff = row->size < win->view.x ? 0 : row->size - win->view.x;
    int maxYoff =
        buf->file.lines < win->view.y ? 0 : buf->file.lines - win->view.y;
    win->view.xoff = RANGE(win->view.xoff, 0, maxXoff);
    win->view.yoff = RANGE(win->view.yoff, 0, maxYoff);
    win_render_rows(win);
}

void cursor_right(Win *win) {
    win->cur.x++;
    handleScroll(win);
    cursor_check(win);
    cursor_refresh(win);
}

void cursor_left(Win *win) {
    win->cur.x--;
    handleScroll(win);
    cursor_check(win);
    cursor_refresh(win);
}

void cursor_up(Win *win) {
    win->cur.y--;
    handleScroll(win);
    cursor_check(win);
    cursor_refresh(win);
}

void cursor_down(Win *win) {
    win->cur.y++;
    handleScroll(win);
    cursor_check(win);
    cursor_refresh(win);
}
