// normal.c: handle normal mode

#include "normal.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "fileio.h"
#include "key.h"
#include "message.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include "window.h"

#include <ctype.h>
#include <string.h>

extern Win *curwin;
extern Buffer *curbuf;

static void new_line_above() {
    line_insert_before(curbuf->curline, NULL, 0);
    curbuf->icol = 0;
    curbuf->curline = curbuf->curline->prev;
    update_top_line(curwin);
    win_render_lines(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}

static void new_line_below() {
    line_insert_after(curbuf->curline, NULL, 0);
    curbuf->icol = 0;
    curbuf->iline++;
    curbuf->curline = curbuf->curline->next;
    win_render_lines(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}

static void join_line() {
    curbuf->icol = curbuf->curline->size;
    edit_join_line(curbuf->curline);
    win_render_lines(curwin);
    cursor_refresh(curwin);
}

static void del_end() {
    edit_del_str(curbuf->curline, curbuf->icol,
                 curbuf->curline->size - curbuf->icol);
    win_render_lines(curwin);
}

static void del_line() {
    line_remove(curbuf->curline);
    win_render_lines(curwin);
}

static void replace_char() {
    char c = getch();
    /* TODO: fix special character */
    if (c == ESC) {
        return;
    }
    Line *line = curbuf->curline;
    line->content[curbuf->icol] = c;
    win_render_lines(curwin);
}

static void del_char() {
    edit_del_char(curbuf->curline, curbuf->icol);
    win_render_lines(curwin);
    cursor_refresh(curwin);
}

static void move_end_line() {
    curbuf->icol = curbuf->curline->size;
    cursor_refresh(curwin);
}

static void move_start_line() {
    curbuf->icol = 0;
    cursor_refresh(curwin);
}

static void move_screen(int lines) {
    curwin->yoff = MAX(0, curwin->yoff + lines);
    curbuf->iline += lines;
    win_scroll(curwin);
}

static void move_cursor(int lines) {
    curbuf->iline = curwin->yoff + lines;
    cursor_refresh(curwin);
}

static void go_top() {

    curbuf->iline = 0;
    curbuf->curline = curbuf->head;
    win_scroll(curwin);
    win_render_num(curwin);
    win_render_status(curwin);
    cursor_refresh(curwin);
}

static void go_bottom() {
    curbuf->iline = curbuf->nlines - 1;
    curbuf->curline = curbuf->tail;
    win_scroll(curwin);
    win_render_num(curwin);
    win_render_status(curwin);
    cursor_refresh(curwin);
}

static void show_info() {
    mess_send("\"%s\" %d lines, --%d%%--", curbuf->name, curbuf->nlines,
              buffer_progress(curbuf));
}

static void swapchar() {
    Line *line = curbuf->curline;
    char *c = &line->content[curbuf->icol];
    *c = islower(*c) ? toupper(*c) : tolower(*c);
    win_render_lines(curwin);
}

static int nextrune(int inc) {
    int n;
    for (n = curbuf->icol + inc;
         n + inc >= 0 && (curbuf->curline->content[n] & 0xc0) == 0x80; n += inc)
        ;
    return n;
}

static void move_word_forward() {
    char *text = curbuf->curline->content;
    if (curbuf->icol > curbuf->curline->size) {
        curbuf->icol = 0;
        curbuf->iline++;
        curbuf->curline = curbuf->curline->next;
    } else {
        while (strchr(" ", text[curbuf->icol])) {
            curbuf->icol++;
        }
        while (!strchr(" ", text[curbuf->icol])) {
            curbuf->icol++;
        }
        curbuf->icol++;
    }

    win_render_lines(curwin);
}

static void move_end_word_forward() {
    char *text = curbuf->curline->content;
    curbuf->icol++;
    if (curbuf->icol > curbuf->curline->size) {
        curbuf->icol = 0;
        curbuf->iline++;
        curbuf->curline = curbuf->curline->next;
    } else {
        while (strchr(" ", text[curbuf->icol])) {
            curbuf->icol++;
        }
        while (!strchr(" ", text[curbuf->icol])) {
            curbuf->icol++;
        }
        curbuf->icol--;
    }

    win_render_lines(curwin);
}

static void move_word_backward() {
    char *text = curbuf->curline->content;
    if (curbuf->icol <= 0) {
        curbuf->iline--;
        curbuf->curline = curbuf->curline->prev;
        curbuf->icol = curbuf->curline->size;
    } else {
        while (strchr(" ", text[nextrune(-1)])) {
            curbuf->icol--;
        }
        while (!strchr(" ", text[nextrune(-1)])) {
            curbuf->icol--;
        }
    }
    win_render_lines(curwin);
}

void normal_mode(const int c) {
    switch (c) {
    // MOVE
    case KEY_LEFT:
    case 'h':
        cursor_left(curwin);
        break;
    case KEY_RIGHT:
    case 'l':
        cursor_right(curwin);
        break;
    case KEY_DOWN:
    case 'j':
        cursor_down(curwin);
        break;
    case KEY_UP:
    case 'k':
        cursor_up(curwin);
        break;
    case 'G':
        go_bottom();
        break;
    case 'g':
        if (getch() == 'g') {
            go_top();
        }
        break;
    case CTRL('d'):
        move_screen(+curwin->wtext_lines / 2);
        break;
    case CTRL('u'):
        move_screen(-curwin->wtext_lines / 2);
        break;
    case CTRL('e'):
        move_screen(-1);
        break;
    case CTRL('y'):
        move_screen(+1);
        break;
    case CTRL('g'):
        show_info();
        break;
    case CTRL('o'):
        /* TODO move backword jump history */
        break;
    case CTRL('i'):
        /* TODO move forward jump history */
        break;
    case 'H':
        move_cursor(0);
        break;
    case 'M':
        move_cursor(curwin->wtext_lines / 2 - 1);
        break;
    case 'L':
        move_cursor(curwin->wtext_lines - 1);
        break;
    case 'w':
        move_word_forward();
        break;
    case 'b':
        move_word_backward();
        break;
    case 'e':
        move_end_word_forward();
        break;
    case 'Z':
        if (getch() == 'Q') {
            viti_quit();
        }
        break;
    case '0':
        move_start_line();
        break;
    case '$':
        move_end_line();
        break;

    // SEARCH
    case 'n':
        search_next();
        break;
    case 'N':
        search_prev();
        break;

    // EDIT
    case 'd':
        if (getch() == 'd') {
            del_line();
        }
        break;
    case 'D':
        del_end();
        break;
    case 'C':
        del_end();
        mode_switch(MODE_INSERT);
        break;
    case 'r':
        replace_char();
        break;
    case 'S':
        edit_del_str(curbuf->curline, 0, curbuf->curline->size);
        win_render_lines(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'J':
        join_line();
        break;
    case 'x':
        del_char();
        break;
    case 'I':
        move_end_line();
        mode_switch(MODE_INSERT);
        break;
    case 's':
        del_char();
        mode_switch(MODE_INSERT);
        break;
    case 'a':
        cursor_right(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'A':
        move_start_line();
        mode_switch(MODE_INSERT);
        break;
    case 'O':
        new_line_above();
        break;
    case 'o':
        new_line_below();
        break;
    case '~':
        swapchar();
        break;

    // MODE
    case 'i':
        mode_switch(MODE_INSERT);
        break;
    case ':':
        mode_switch(MODE_COMMAND);
        break;
    case '/':
        mode_switch(MODE_SEARCH);
        break;
    }
}
