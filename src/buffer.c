#include "buffer.h"
#include "mess.h"
#include "util.h"
#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

Buffer *cbuf;
Buffer *Bufs;

Row *current_row(Buffer *buf) { return &buf->rows[current_line(buf)]; }
int current_line(Buffer *buf) { return buf->cur.y + buf->view.yoff; }
int current_col(Buffer *buf) { return buf->cur.x + buf->view.xoff; }

Buffer buffer_create(int height, int width, int y, int x) {
    Buffer buf;
    buf.file.lines = 1;
    buf.file.name = NULL;

    int size_statusline = 1;
    int size_numbercol = 4;

    buf.cur.x = 0;
    buf.cur.y = 0;
    buf.rows = malloc(sizeof(Row));
    buf.rows[0].content = malloc(sizeof(char));

    buf.view.xoff = 0;
    buf.view.yoff = 0;
    buf.view.x = width - size_numbercol;
    buf.view.y = height - size_statusline;
    buf.win = newwin(buf.view.y, buf.view.x, y, x + size_numbercol);
    keypad(buf.win, TRUE);

    buf.numbercol = newwin(height - size_statusline, size_numbercol, y, x);

    buf.statusline =
        newwin(size_statusline, width, height - size_statusline, x);
    refresh();
    return buf;
}

void buffer_render_rows(Buffer *buf) {
    werase(buf->win);
    for (int y = 0; y < MIN(buf->file.lines, buf->view.y); y++) {
        Row *row = &buf->rows[buf->view.yoff + y];
        int cols = MIN(row->size, buf->view.x);
        for (int x = 0; x < cols; x++) {
            mvwaddch(buf->win, y, x, row->content[buf->view.xoff + x]);
        }
    }
    wrefresh(buf->win);
}

void buffer_render_numbercol(Buffer *buf) {
    werase(buf->numbercol);

    for (int y = 0; y < buf->view.y; y++) {
        if (y + buf->view.yoff >= buf->file.lines) {
            mvwaddstr(buf->numbercol, y, 0, "~");
            continue;
        }

        char num[20];
        if (y == buf->cur.y) {
            sprintf(num, "%d", y + buf->view.yoff);
        } else {
            sprintf(num, "%d", abs(buf->cur.y - y));
        }
        mvwaddstr(buf->numbercol, y, 1, num);
    }
    wrefresh(buf->numbercol);
}

void buffer_render_statusline(Buffer *buf) {
    werase(buf->statusline);
    wbkgd(buf->statusline, A_REVERSE);
    waddstr(buf->statusline,
            buf->file.name != NULL ? buf->file.name : "[NONAME]");

    char lineinfo[128];
    char percentInfo[128];
    sprintf(percentInfo, "%.f%%",
            (float)current_line(cbuf) / buf->file.lines * 100);
    sprintf(lineinfo, "%d,%d", current_line(buf), buf->file.lines);

    mvwaddstr(buf->statusline, 0,
              COLS - strlen(lineinfo) - 5 - strlen(percentInfo), lineinfo);
    mvwaddstr(buf->statusline, 0, COLS - strlen(percentInfo), percentInfo);

    wrefresh(buf->statusline);
}

void buffer_render(Buffer *buf) {
    buffer_render_numbercol(buf);
    buffer_render_rows(buf);
    buffer_render_statusline(buf);
}
