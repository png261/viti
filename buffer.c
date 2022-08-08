#include "buffer.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

Buffer buffer_create(int height, int width, int y, int x) {
    Buffer buf;
    buf.file.lines = 0;
    buf.file.name = "[NONAME]";

    int size_statusline = 1;
    int size_numbercol = 4;

    buf.cur.x = 0;
    buf.cur.y = 0;

    buf.view.xoff = 0;
    buf.view.yoff = 0;
    buf.view.line = 0;
    buf.view.x = width - size_numbercol;
    buf.view.y = height - size_statusline;
    buf.win = newwin(buf.view.y, buf.view.x, y, x + size_numbercol);

    buf.numbercol = newwin(height - size_statusline, size_numbercol, y, x);

    buf.statusline =
        newwin(size_statusline, width, height - size_statusline, x);
    refresh();
    return buf;
}

void buffer_render_rows(Buffer *buf) {
    werase(buf->win);
    int lines = MIN(buf->file.lines, buf->view.y);

    for (int y = 0; y < lines; y++) {
        Row current = buf->rows[buf->view.yoff + y];
        int cols = MIN(current.size, buf->view.x);
        for (int x = 0; x < cols; x++) {
            mvwaddch(buf->win, y, x, current.content[buf->view.xoff + x]);
        }
    }
    wrefresh(buf->win);
}

void buffer_render_numbercol(Buffer *buf) {
    werase(buf->numbercol);
    int cols = log10(buf->file.lines) + 1;

    for (int y = 0; y < buf->view.y; y++) {
        if (y + buf->view.yoff > buf->file.lines) {
            mvwaddstr(buf->numbercol, y, 0, "~");
            continue;
        }
        char *num = malloc(cols * sizeof(char));

        if (y == buf->cur.y) {
            sprintf(num, "%d", y + buf->view.yoff);
        } else {
            sprintf(num, "%d", abs(buf->cur.y - y));
        }
        mvwaddstr(buf->numbercol, y, 1, num);
        free(num);
    }
    wrefresh(buf->numbercol);
}

void buffer_render_statusline(Buffer *buf) {
    werase(buf->statusline);
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);
    wbkgd(buf->statusline, COLOR_PAIR(1));
    waddstr(buf->statusline, buf->file.name);
    char lineinfo[20];
    sprintf(lineinfo, "%d/%d", buf->view.line + 1, buf->file.lines);
    mvwaddstr(buf->statusline, 0, COLS - strlen(lineinfo), lineinfo);
    wrefresh(buf->statusline);
}

void buffer_render(Buffer *buf) {
    buffer_render_numbercol(buf);
    buffer_render_rows(buf);
    buffer_render_statusline(buf);
}
