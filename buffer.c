#include "buffer.h"

void buffer_render_num(Buffer *buf) {
    for (int y = 0; y < buf->view.y; y++) {
        mvwaddstr(buf->num, y, 0, "~");
    }
    wrefresh(buf->num);
}

void buffer_render_rows(Buffer *buf) {
    int maxy = buf->nlines < buf->view.y ? buf->nlines : buf->view.y;
    for (int y = 0; y < maxy; y++) {
        row current = buf->rows[buf->view.yoff + y];
        int maxx = current.size < buf->view.x ? current.size : buf->view.x;
        for (int x = 0; x < maxx; x++) {
            mvwaddch(buf->win, y, x, current.content[buf->view.xoff + x]);
        }
    }
    wrefresh(buf->win);
}

void buffer_render_statusline(Buffer *buf) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    wbkgd(buf->statusline, COLOR_PAIR(1));
    waddstr(buf->statusline, buf->filename);
    wrefresh(buf->statusline);
}

void buffer_render(Buffer *buf) {
    buffer_render_num(buf);
    buffer_render_rows(buf);
    buffer_render_statusline(buf);
}

Buffer buffer_create(int height, int width, int y, int x) {
    Buffer buf;
    buf.view.xoff = 0;
    buf.view.yoff = 0;
    buf.nlines = 0;
    buf.view.x = width;
    buf.view.y = height;
    buf.filename = "[NONAME]";
    int size_statusline = 1;
    int size_num = 1;
    buf.win =
        newwin(height - size_statusline, width - size_num, y, x + size_num);
    buf.num = newwin(height - size_statusline, size_num, y, x);
    buf.statusline =
        newwin(size_statusline, width, height - size_statusline, x);
    refresh();
    return buf;
}
