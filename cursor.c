#include "buffer.h"
#include "cursor.h"
#include "util.h"
#include "mess.h"

void cursor_refresh(Buffer *buf) {
    View *view = &buf->view;
    view->line = buf->cur.y + view->yoff;
    view->col = buf->cur.x + view->xoff;
    buffer_render_numbercol(buf);
    buffer_render_statusline(buf);
    wmove(buf->win, buf->cur.y, buf->cur.x);
    wrefresh(buf->win);
}

void cursor_check(Buffer *buf) {
    Row *row = &buf->rows[buf->view.line];
    buf->cur.x = RANGE(buf->cur.x, 0,
                       (row->size < buf->view.x ? row->size : buf->view.x - 1));
    buf->cur.y = RANGE(buf->cur.y, 0, MIN(buf->file.lines, buf->view.y - 1));
}

void handleScroll(Buffer *buf) {
    Row *row = &buf->rows[buf->view.line];
    if (buf->cur.x < 0) {
        buf->view.xoff--;
    }

    if (buf->cur.x >= buf->view.x) {
        buf->view.xoff += buf->cur.x + 1 - buf->view.x;
    }

    if (buf->cur.y < 0) {
        buf->view.yoff--;
    }

    if (buf->cur.y >= buf->view.y) {
        buf->view.yoff += buf->cur.y + 1 - buf->view.y;
    }

    buf->view.xoff =
        RANGE(buf->view.xoff, 0,
              row->size < buf->view.x ? 0 : row->size - buf->view.x);
    buf->view.yoff = RANGE(
        buf->view.yoff, 0,
        buf->file.lines < buf->view.y ? 0 : buf->file.lines - buf->view.y);
    buffer_render_rows(buf);
}

void cursor_right(Buffer *buf) {
    buf->cur.x++;
    handleScroll(buf);
    cursor_check(buf);
    cursor_refresh(buf);
}

void cursor_left(Buffer *buf) {
    buf->cur.x--;
    handleScroll(buf);
    cursor_check(buf);
    cursor_refresh(buf);
}

void cursor_up(Buffer *buf) {
    buf->cur.y--;
    handleScroll(buf);
    cursor_check(buf);
    cursor_refresh(buf);
}

void cursor_down(Buffer *buf) {
    buf->cur.y++;
    handleScroll(buf);
    cursor_check(buf);
    cursor_refresh(buf);
}
