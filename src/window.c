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
#include <math.h>
#include <ncurses.h>


Win *curwin;
extern Buffer *curbuf;
extern char *search_query;


void update_top_line(Win *win)
{
    Line * line = curbuf->curline;
    int count = curbuf->iline - win->yoff;
    while(count--){
        if(line->prev == NULL) {
            break;
        }
        line = line->prev;  
    }
    win->top_line = line;
}


void win_scroll(Win *win) 
{
    Buffer *buf = win->buf;

    LIMIT(buf->iline, 0, buf->nlines - 1);
    LIMIT(buf->icol, 0, buf->curline == NULL ? 0 : buf->curline->size);

    if (buf->icol < win->xoff) {
        win->xoff = buf->icol;
        update_top_line(win);
        win_render_lines(win);
    }
    if (buf->icol >= win->xoff + win->wtext_cols) {
        win->xoff = buf->icol - win->wtext_cols + 1;
        update_top_line(win);
        win_render_lines(win);
    }

    if (buf->iline < win->yoff) {
        win->yoff = buf->iline;
        update_top_line(win);
        win_render_lines(win);
    }
    if (buf->iline >= win->yoff + win->wtext_lines) {
        win->yoff = buf->iline - win->wtext_lines + 1;
        update_top_line(win);
        win_render_lines(win);
    }
}


Win *win_resize(Win * win, int lines, int cols)
{
    win->size.lines = lines;
    win->size.cols = cols;
    win->wtext_lines = lines - win->wstatus_lines;
    win->wtext_cols = cols - win->wnum_cols;

    wresize(win->wtext, win->wtext_lines, cols);
    wresize(win->wstatus, win->wstatus_lines, cols);
    wresize(win->wnum, win->wnum_lines, win->wnum_cols);

    mvwin(win->wstatus, lines - win->wstatus_lines, 0);
    mvwin(win->wtext, 0, win->wnum_cols);

    wrefresh(win->wtext);
    wrefresh(win->wnum);
    wrefresh(win->wstatus);
}


Win *win_create(Buffer *buf, int lines, int cols, int y, int x) 
{
    Win *win = xmalloc(sizeof(*win));

    if (buf == NULL) {
        win->buf = buffer_create();
    } else {
        win->buf = buf;
    }

    win->xoff = 0;
    win->yoff = 0;

    win->size.lines = lines;
    win->size.cols = cols;

    win->wstatus_lines = 1;
    win->wstatus_cols = cols;
    win->wstatus = newwin(win->wstatus_lines, win->wstatus_cols, lines - win->wstatus_lines, x);

    win->wnum_lines = lines;
    win->wnum_cols = 6;
    win->wnum = newwin(lines - win->wnum_lines, win->wnum_cols, y, x);

    win->wtext_lines = lines - win->wstatus_lines;
    win->wtext_cols = cols - win->wnum_cols;
    win->wtext = newwin(win->wtext_lines, win->wtext_cols, y, x + win->wnum_cols);

    keypad(win->wtext, TRUE);
    refresh();

    return win;
}

void print_lines(Win *win)
{
    Line *line = win->top_line;
    for (int y = 0; y < win->wtext_lines; y++) {
        if(line == NULL) {
            return; 
        }

        int len = MIN(MAX(line->size - win->xoff, 0), win->wtext_cols);
        mvwaddnstr(win->wtext, y, 0, &line->content[win->xoff], len);
        line = line->next;
    }
}


static void win_update_highlight(Win *win) 
{
    if (!is_highlight || search_query == NULL) {
        return;
    }

    for (int y = 0; y <  win->wtext_lines; y++) {
        char line[win->wtext_cols];
        mvwinnstr(win->wtext, y, 0, line, win->wtext_cols); 
        highlight_line(win->wtext, line, search_query, PAIR_HIGHLIGHT, y);
    }
}

void win_render_lines(Win *win) 
{
    werase(win->wtext);

    curbuf->nlines = MAX(1, curbuf->nlines);
    print_lines(win);
    win_update_highlight(win);
    win_scroll(win);
    cursor_refresh(win);

    wrefresh(win->wtext);
}


void win_render_numbercol(Win *win) 
{
    Buffer *buf = win->buf;
    werase(win->wnum);
    wattron(win->wnum, COLOR_PAIR(PAIR_NUMBERCOL));

    for (int y = 0; y < win->wtext_lines; y++) {
        if (y + win->yoff > buf->nlines - 1) {
            mvwaddch(win->wnum, y, 0, '~');
            continue;
        } 
        char num[20];
        int cury = buf->iline - win->yoff;
        if (y == cury) {
            sprintf(num, "%d", buf->iline) ;
            mvwaddstr(win->wnum, y, 0, num);
            continue;
        }  
        sprintf(num, "%d", abs(cury - y));
        mvwaddstr(win->wnum, y, 2, num);
    }

    wattroff(win->wnum, COLOR_PAIR(PAIR_NUMBERCOL));
    wrefresh(win->wnum);
}


void win_render_statusline(Win *win) 
{
    Buffer *buf = win->buf;
    werase(win->wstatus);
    wbkgd(win->wstatus, A_REVERSE);

    waddstr(win->wstatus, buf->name != NULL ? buf->name : "[NONAME]");

    char lineinfo[128];
    char percentInfo[128];

    sprintf(lineinfo, "%d,%d", buf->iline + 1, buf->nlines);
    sprintf(percentInfo, "%d%%", buffer_progress(win->buf));

    mvwaddstr(win->wstatus, 0, win->size.cols - strlen(lineinfo) - 5 - strlen(percentInfo), lineinfo);
    mvwaddstr(win->wstatus, 0, win->size.cols - strlen(percentInfo), percentInfo);

    wrefresh(win->wstatus);
}


void win_render(Win *win) 
{
    win_render_numbercol(win);
    win_render_statusline(win);
    win_render_lines(win);
}
