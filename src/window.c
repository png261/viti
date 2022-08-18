#include "window.h"

#include "color.h"
#include "highlight.h"
#include "mess.h"
#include "search.h"
#include "stdlib.h"
#include "util.h"
#include <string.h>

Win *cwin;
extern char *search_query;

int current_line(Win *win) { return win->buf->line; }
int current_col(Win *win) { return win->buf->col; }
int buffer_progress(Win *win) {
    return (int)((float)(current_line(win) + 1) / win->buf->file.lines * 100);
}

Win *win_create(Buffer *buf, int height, int width, int y, int x) {
    Win *win = malloc(sizeof(Win));

    if (buf == NULL) {
        win->buf = buffer_create();
    } else {
        win->buf = buf;
    }

    int size_statusline = 1;
    int size_numbercol = 6;

    win->view.xoff = 0;
    win->view.yoff = 0;
    win->view.x = width - size_numbercol;
    win->view.y = height - size_statusline;

    win->textarea = newwin(win->view.y, win->view.x, y, x + size_numbercol);
    keypad(win->textarea, TRUE);
    win->numbercol = newwin(height - size_statusline, size_numbercol, y, x);
    win->statusline =
        newwin(size_statusline, width, height - size_statusline, x);
    refresh();
    return win;
}

void print_rows(Win *win) {
    Buffer *buf = win->buf;
    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        Row *row = &buf->rows[win->view.yoff + y];
        for (int x = 0; x < MIN(row->size - win->view.xoff, win->view.x); x++) {
            mvwaddch(win->textarea, y, x, row->content[win->view.xoff + x]);
        }
    }
}

void win_update_highlight(Win *win) {
    if (!is_highlight || search_query == NULL) {
        return;
    }

    Buffer *buf = win->buf;
    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        highlight_row(win, win->view.yoff + y, search_query, PAIR_HIGHLIGHT);
    }
}

void win_render_rows(Win *win) {
    wclear(win->textarea);

    print_rows(win);
    win_update_highlight(win);

    wrefresh(win->textarea);
}

void win_render_numbercol(Win *win) {
    Buffer *buf = win->buf;
    werase(win->numbercol);
    wattron(win->numbercol, COLOR_PAIR(PAIR_NUMBERCOL));

    for (int y = 0; y < win->view.y; y++) {
        if (y + win->view.yoff >= buf->file.lines) {
            mvwaddch(win->numbercol, y, 0, '~');
            continue;
        }
        char num[20];
        if (y == win->buf->line) {
            sprintf(num, "%d", y + win->view.yoff + 1);
            mvwaddstr(win->numbercol, y, 0, num);

        } else {
            sprintf(num, "%d", abs(win->buf->line - y));
            mvwaddstr(win->numbercol, y, 2, num);
        }
    }
    wattroff(win->numbercol, COLOR_PAIR(PAIR_NUMBERCOL));
    wrefresh(win->numbercol);
}

void win_render_statusline(Win *win) {
    Buffer *buf = win->buf;
    werase(win->statusline);
    wbkgd(win->statusline, COLOR_PAIR(PAIR_STATUSLINE));
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
