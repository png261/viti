#include "buffer.h"
#include "cursor.h"
#include "util.h"

void cursor_refresh(Buffer *buf) {
    View *view = &buf->view;
    view->line = buf->cur.y + view->yoff;
    view->col = buf->cur.x + view->xoff;
    buffer_render_numbercol(buf);
    buffer_render_statusline(buf);
    wmove(buf->win, buf->cur.y, buf->cur.x);
    wrefresh(buf->win);
}

void cursor_right(Buffer *buf) {
    if (buf->cur.x + 1 >= buf->view.x) {
        buf->view.xoff =
            MIN(buf->view.xoff++, buf->rows[buf->cur.y].size - buf->view.x);
        buffer_render_rows(buf);
    }
    buf->cur.x = MIN(buf->cur.x++, buf->view.x - 1);
    cursor_refresh(buf);
}

void cursor_left(Buffer *buf) {
    if (buf->cur.x <= 0) {
        buf->view.xoff = MAX(buf->view.xoff--, 0);
        buffer_render_rows(buf);
    }
    buf->cur.x = MAX(buf->cur.x--, 0);
    cursor_refresh(buf);
}

void cursor_up(Buffer *buf) {
    if (buf->cur.y <= 0) {
        buf->view.yoff = MAX(buf->view.yoff--, 0);
        buffer_render_rows(buf);
    }
    buf->cur.y = MAX(buf->cur.y--, 0);
    cursor_refresh(buf);
}

void cursor_down(Buffer *buf) {
    if (buf->cur.y + 1 >= buf->view.y) {
        buf->view.yoff = MIN(buf->view.yoff++, buf->file.lines - buf->view.y);
        buffer_render_rows(buf);
    }
    buf->cur.y = MIN(buf->cur.y++, buf->view.y - 1);
    cursor_refresh(buf);
}
