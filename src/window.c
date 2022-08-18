#include "window.h"

#include "color.h"
#include "mess.h"
#include "search.h"
#include "stdlib.h"
#include "util.h"
#include <string.h>

Win *cwin;
extern MatchedList *matched_list;

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

void win_render_rows(Win *win) {
    wclear(win->textarea);

    Buffer *buf = win->buf;
    Matched *matched = NULL;
    if (matched_list != NULL) {
        matched = matched_list->matched;
    }

    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        int line = win->view.yoff + y;
        Row *row = &buf->rows[line];
        int cols = MIN(row->size - win->view.xoff, win->view.x);
        for (int x = 0; x < cols; x++) {
            int col = win->view.xoff + x;
            if (matched != NULL && matched->y == line && col >= matched->x &&
                col < (matched->x + matched_list->len)) {
                wattron(win->textarea, COLOR_PAIR(PAIR_HIGHLIGHT));
                mvwaddch(win->textarea, y, x, row->content[col]);
                wattroff(win->textarea, COLOR_PAIR(PAIR_HIGHLIGHT));
            } else {
                int col = win->view.xoff + x;
                mvwaddch(win->textarea, y, x, row->content[col]);
            }
        }
        if (matched != NULL && line == matched->y) {
            matched++;
        }
    }

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
