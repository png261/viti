#include "stdlib.h"
#include "util.h"
#include "window.h"
#include <string.h>

Win *cwin;

int current_line(Win *win) { return win->cur.y + win->view.yoff; }
int current_col(Win *win) { return win->cur.x + win->view.xoff; }
int buffer_progress(Win *win) {
    return (int)((float)(current_line(win) + 1) / win->buf->file.lines * 100);
}

Win *win_create(int height, int width, int y, int x) {
    Win *win = malloc(sizeof(Win));

    int size_statusline = 1;
    int size_numbercol = 6;
    win->buf = NULL;

    win->view.xoff = 0;
    win->view.yoff = 0;
    win->view.x = width - size_numbercol;
    win->view.y = height - size_statusline;

    win->textarea = newwin(win->view.y, win->view.x, y, x + size_numbercol);
    win->numbercol = newwin(height - size_statusline, size_numbercol, y, x);
    win->statusline =
        newwin(size_statusline, width, height - size_statusline, x);

    keypad(win->textarea, TRUE);
    refresh();
    return win;
}

void win_render_rows(Win *win) {
    Buffer *buf = win->buf;
    werase(win->textarea);
    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        Row *row = &buf->rows[win->view.yoff + y];
        int cols = MIN(row->size, win->view.x);
        for (int x = 0; x < cols; x++) {
            mvwaddch(win->textarea, y, x, row->content[win->view.xoff + x]);
        }
    }
    wrefresh(win->textarea);
}

void win_render_numbercol(Win *win) {
    Buffer *buf = win->buf;
    werase(win->numbercol);

    for (int y = 0; y < win->view.y; y++) {
        if (y + win->view.yoff >= buf->file.lines) {
            mvwaddstr(win->numbercol, y, 0, "~");
            continue;
        }

        char num[20];
        if (y == win->cur.y) {
            sprintf(num, "%d", y + win->view.yoff + 1);
            mvwaddstr(win->numbercol, y, 0, num);
        } else {
            sprintf(num, "%d", abs(win->cur.y - y));
            mvwaddstr(win->numbercol, y, 2, num);
        }
    }
    wrefresh(win->numbercol);
}

void win_render_statusline(Win *win) {
    Buffer *buf = win->buf;
    werase(win->statusline);
    wbkgd(win->statusline, A_REVERSE);
    waddstr(win->statusline,
            buf->file.name != NULL ? buf->file.name : "[NONAME]");

    char lineinfo[128];
    char percentInfo[128];
    sprintf(percentInfo, "%d%%", buffer_progress(win));
    sprintf(lineinfo, "%d,%d", current_line(win) + 1, buf->file.lines);

    mvwaddstr(win->statusline, 0,
              COLS - strlen(lineinfo) - 5 - strlen(percentInfo), lineinfo);
    mvwaddstr(win->statusline, 0, COLS - strlen(percentInfo), percentInfo);

    wrefresh(win->statusline);
}

void win_render(Win *win) {
    win_render_numbercol(win);
    win_render_rows(win);
    win_render_statusline(win);
}
