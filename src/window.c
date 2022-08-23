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
#include <panel.h>


Win *curwin;
extern char *search_query;

void win_scroll(Win *win) 
{
    Buffer *buf = win->buf;
    Line *line = current_line(win);

    LIMIT(buf->curcol, 0, line->size);
    LIMIT(buf->curline, 0, buf->nlines - 1);

    if (buf->curcol < win->view.xoff) {
        win->view.xoff = buf->curcol;
        win_render_lines(win);
    }
    if (buf->curcol >= win->view.xoff + win->view.x) {
        win->view.xoff = buf->curcol - win->view.x + 1;
        win_render_lines(win);
    }

    if (buf->curline < win->view.yoff) {
        win->view.yoff = buf->curline;
        win_render_lines(win);
    }
    if (buf->curline >= win->view.yoff + win->view.y) {
        win->view.yoff = buf->curline - win->view.y + 1;
        win_render_lines(win);
    }
}

int buffer_progress(Win *win)
{
    return (int)((float)(win->buf->curline + 1) / win->buf->nlines * 100);
}

Line *current_line(Win *win) 
{ 
    return &win->buf->lines[win->buf->curline];
}

Win *win_resize(Win * win, const int height, const int width)
{
    const int size_statusline = 1;
    const int size_numbercol = 6;
    win->view.x = width - size_numbercol;
    win->view.y = height - size_statusline;
    wresize(win->textarea, win->view.y, win->view.x );
    wresize(win->numbercol, win->view.y, size_numbercol);
    wresize(win->statusline, size_statusline, width);

    mvwin(win->textarea, 0, size_numbercol);
    mvwin(win->statusline, LINES - 2, 0);

    wrefresh(win->textarea);
    wrefresh(win->numbercol);
    wrefresh(win->statusline);
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

void print_lines(Win *win, Line *lines)
{
    Buffer *buf = win->buf;
    for (int y = 0; y < MIN(buf->nlines, win->view.y); y++) {
        Line *line = &buf->lines[win->view.yoff + y];
        int len = MIN(MAX(line->size - win->view.xoff, 0), win->view.x);
        mvwaddnstr(win->textarea, y, 0, &line->content[win->view.xoff], len);
    }
}

void win_update_highlight(Win *win) 
{
    if (!is_highlight || search_query == NULL) {
        return;
    }

    Buffer *buf = win->buf;
    Line *line = &buf->lines[win->view.yoff];
    for (int y = 0; y < MIN(buf->nlines, win->view.y); y++) {
        highlight_line(win, line + y, search_query, PAIR_HIGHLIGHT);
    }
}

void win_clear_line(Win * win, int line){
    wmove(win->textarea, line - win->view.yoff, 0);
    wclrtoeol(win->textarea);
}

void win_render_line(Win * win, Line *line){
    if(line == NULL){
        return; 
    }

    win_clear_line(win, line - win->buf->lines);

    int len = MIN(MAX(line->size - win->view.xoff, 0), win->view.x);
    mvwaddnstr(win->textarea, line - win->buf->lines - win->view.yoff, 0, &line->content[win->view.xoff], len);
    win_scroll(win);
    cursor_refresh(win);
    wrefresh(win->textarea);
}

void win_render_lines(Win *win) 
{
    werase(win->textarea);

    print_lines(win, win->buf->lines);
    win_update_highlight(win);
    win_scroll(win);
    cursor_refresh(win);

    wrefresh(win->textarea);
}

void relative_number(Win *win, int y) 
{
    Buffer *buf = win->buf; 
    char num[20];
    if (y == buf->curline - win->view.yoff) {
        sprintf(num, "%d", y + win->view.yoff + 1);
        mvwaddstr(win->numbercol, y, 0, num);
        return;
    } 

    sprintf(num, "%d", abs(buf->curline - win-> view.yoff - y));
    mvwaddstr(win->numbercol, y, 2, num);
}

void win_render_numbercol(Win *win) 
{
    Buffer *buf = win->buf;
    werase(win->numbercol);
    wattron(win->numbercol, COLOR_PAIR(PAIR_NUMBERCOL));

    for (int y = 0; y < win->view.y; y++) {
        if (y + win->view.yoff >= buf->nlines) {
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

    sprintf(lineinfo, "%d,%d", buf->curline + 1, buf->nlines);
    sprintf(percentInfo, "%d%%", buffer_progress(win));

    mvwaddstr(win->statusline, 0,
              COLS - strlen(lineinfo) - 5 - strlen(percentInfo), lineinfo);

    mvwaddstr(win->statusline, 0, COLS - strlen(percentInfo), percentInfo);

    wrefresh(win->statusline);
}

void win_render(Win *win) 
{
    win_render_numbercol(win);
    win_render_lines(win);
    win_render_statusline(win);
}
