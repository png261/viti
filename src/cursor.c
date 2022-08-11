#include "buffer.h"
#include "cursor.h"
#include "mess.h"
#include "util.h"

void cursor_refresh(Buffer *buf) {
    buffer_render_numbercol(buf);
    buffer_render_statusline(buf);
    wmove(buf->win, buf->cur.y, buf->cur.x);
    wrefresh(buf->win);
}

void cursor_check(Buffer *buf) {
    Row *row = current_row(buf);
    int maxX = MIN(row->size < 0 ? 0 : row->size, buf->view.x - 1);
    int maxY = MIN(buf->file.lines, buf->view.y) - 1;

    buf->cur.x = RANGE(buf->cur.x, 0, maxX);
    buf->cur.y = RANGE(buf->cur.y, 0, maxY);
    mess_send("%d/%d", buf->cur.x, maxX);
}

void handleScroll(Buffer *buf) {
    Row *row = current_row(buf);
    if (buf->cur.x < 0) {
        buf->view.xoff += buf->cur.x;
    }

    if (buf->cur.x >= buf->view.x) {
        buf->view.xoff += buf->cur.x + 1 - buf->view.x;
    }

    if (buf->cur.y < 0) {
        buf->view.yoff += buf->cur.y;
    }

    if (buf->cur.y >= buf->view.y) {
        buf->view.yoff += buf->cur.y + 1 - buf->view.y;
    }

    int maxXoff = row->size < buf->view.x ? 0 : row->size - buf->view.x;
    int maxYoff =
        buf->file.lines < buf->view.y ? 0 : buf->file.lines - buf->view.y;
    buf->view.xoff = RANGE(buf->view.xoff, 0, maxXoff);
    buf->view.yoff = RANGE(buf->view.yoff, 0, maxYoff);
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

void go_to_line(Buffer *buf, int line) {
    buf->view.yoff = 0;
    buf->cur.y = line;
    handleScroll(buf);
    cursor_check(buf);
    cursor_refresh(buf);
}
