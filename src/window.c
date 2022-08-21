#include "window.h"

#include "color.h"
#include "buffer.h"
#include "highlight.h"
#include "message.h"
#include "search.h"
#include "stdlib.h"
#include "util.h"
#include "memory.h"

#include <string.h>


Win *curwin;
extern char *search_query;

void win_scroll(Win *win) 
{
    Buffer *buf = win->buf;
    Row *row = current_row(win);

    LIMIT(buf->col, 0, row->size);
    LIMIT(buf->line, 0, buf->file.lines - 1);

    if (buf->col < win->view.xoff) {
        win->view.xoff = buf->col;
        win_render_rows(win);
    }
    if (buf->col >= win->view.xoff + win->view.x) {
        win->view.xoff = buf->col - win->view.x + 1;
        win_render_rows(win);
    }

    if (buf->line < win->view.yoff) {
        win->view.yoff = buf->line;
        win_render_rows(win);
    }
    if (buf->line >= win->view.yoff + win->view.y) {
        win->view.yoff = buf->line - win->view.y + 1;
        win_render_rows(win);
    }
}


int current_line(Win *win)
{
    return win->buf->line; 
}

int current_col(Win *win) 
{
    return win->buf->col; 
}

int buffer_progress(Win *win)
{
    return (int)((float)(win->buf->line + 1) / win->buf->file.lines * 100);
}

Row *current_row(Win *win) 
{ 
    return &win->buf->rows[win->buf->line];
}

Win *win_create(Buffer *buf, const int height, const int width, const int y, const int x) 
{
    Win *win = xmalloc(sizeof(*win));

    if (buf == NULL) {
        win->buf = buffer_create();
    } else {
        win->buf = buf;
    }

    const int size_statusline = 1;
    const int size_numbercol = 6;

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

void print_rows(Win *win)
{
    Buffer *buf = win->buf;
    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        Row *row = &buf->rows[win->view.yoff + y];
        int len = MIN(MAX(row->size - win->view.xoff, 0), win->view.x);
        mvwaddnstr(win->textarea, y, 0, &row->content[win->view.xoff], len);
    }
}

void win_update_highlight(Win *win) 
{
    if (!is_highlight || search_query == NULL) {
        return;
    }

    Buffer *buf = win->buf;
    for (int y = 0; y < MIN(buf->file.lines, win->view.y); y++) {
        highlight_row(win, win->view.yoff + y, search_query, PAIR_HIGHLIGHT);
    }
}

void win_clear_line(Win * win, int line){
    wmove(win->textarea, line - win->view.yoff, 0);
    wclrtoeol(win->textarea);
}

void win_render_line(Win * win, int line){
    Row *row = &win->buf->rows[line];
    if(row == NULL){
        return; 
    }

    win_clear_line(win, line);
    int len = MIN(MAX(row->size - win->view.xoff, 0), win->view.x);
    mvwaddnstr(win->textarea, line - win->view.yoff, 0, &row->content[win->view.xoff], len);
    win_scroll(win);
    cursor_refresh(win);
    wrefresh(win->textarea);
}

void win_render_rows(Win *win) 
{
    werase(win->textarea);

    print_rows(win);
    win_update_highlight(win);
    win_scroll(win);
    cursor_refresh(win);

    wrefresh(win->textarea);
}

void relative_number(Win *win, int y) 
{
    char num[20];
    if (y == win->buf->line - win->view.yoff) {
        sprintf(num, "%d", y + win->view.yoff + 1);
        mvwaddstr(win->numbercol, y, 0, num);
        return;
    } 

    sprintf(num, "%d", abs(win->buf->line - win-> view.yoff - y));
    mvwaddstr(win->numbercol, y, 2, num);
}

void win_render_numbercol(Win *win) 
{
    Buffer *buf = win->buf;
    werase(win->numbercol);
    wattron(win->numbercol, COLOR_PAIR(PAIR_NUMBERCOL));

    for (int y = 0; y < win->view.y; y++) {
        if (y + win->view.yoff >= buf->file.lines) {
            mvwaddch(win->numbercol, y, 0, '~');
        } else {
            relative_number(win, y);
        }
    }

    wattroff(win->numbercol, COLOR_PAIR(PAIR_NUMBERCOL));
    wrefresh(win->numbercol);
}

void win_render_statusline(Win *win) 
{
    Buffer *buf = win->buf;
    werase(win->statusline);
    wbkgd(win->statusline, COLOR_PAIR(PAIR_STATUSLINE));

    waddstr(win->statusline,
            buf->file.name != NULL ? buf->file.name : "[NONAME]");

    char lineinfo[128];
    char percentInfo[128];

    sprintf(lineinfo, "%d,%d", current_line(win) + 1, buf->file.lines);
    sprintf(percentInfo, "%d%%", buffer_progress(win));

    mvwaddstr(win->statusline, 0,
              COLS - strlen(lineinfo) - 5 - strlen(percentInfo), lineinfo);

    mvwaddstr(win->statusline, 0, COLS - strlen(percentInfo), percentInfo);

    wrefresh(win->statusline);
}

void win_render(Win *win) 
{
    win_render_numbercol(win);
    win_render_rows(win);
    win_render_statusline(win);
}
