#include "buffer.h"

void buffer_render(Buffer *buf) {
    for (int i = 0; i < buf->view.y; i++) {
        if (i > buf->nlines || buf->nlines == 0) {
            mvwaddstr(buf->win, i, 0, "~");
        } else {
            mvwaddstr(buf->win, i, 0, buf->rows[buf->view.yoff + i].content);
        }
    }
    wrefresh(buf->win);
}
