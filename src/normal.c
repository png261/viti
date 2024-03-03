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

extern Win *cwin;
extern Buffer *cbuf;

static void new_line_above() {
    line_insert_before(cbuf->cline, NULL, 0);
    cbuf->icol = 0;
    cbuf->cline = cbuf->cline->prev;
    update_tline(cwin);
    win_render_lines(cwin);
    cursor_refresh(cwin);
    mode_switch(MODE_INSERT);
}

static void new_line_below() {
    line_insert_after(cbuf->cline, NULL, 0);
    cbuf->icol = 0;
    cbuf->iline++;
    cbuf->cline = cbuf->cline->next;
    win_render_lines(cwin);
    cursor_refresh(cwin);
    mode_switch(MODE_INSERT);
}

static void join_line() {
    cbuf->icol = cbuf->cline->size;
    edit_join_line(cbuf->cline);
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void del_end() {
    edit_del_str(cbuf->cline, cbuf->icol, cbuf->cline->size - cbuf->icol);
    win_render_lines(cwin);
}

static void del_line() {
    line_remove(cbuf->cline);
    win_render_lines(cwin);
}

static void replace_char() {
    char c = getch();
    /* TODO: fix special character */
    if (c == ESC) {
        return;
    }
    Line *line = cbuf->cline;
    line->content[cbuf->icol] = c;
    win_render_lines(cwin);
}

static void del_char() {
    edit_del_char(cbuf->cline, cbuf->icol);
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void move_end_line() {
    cbuf->icol = cbuf->cline->size;
    cursor_refresh(cwin);
}

static void move_start_line() {
    cbuf->icol = 0;
    cursor_refresh(cwin);
}

static void move_screen(int lines) {
    cwin->lineoff = MAX(0, cwin->lineoff + lines);
    cbuf->iline += lines;
    win_scroll(cwin);
}

static void move_cursor(int lines) {
    cbuf->iline = cwin->lineoff + lines;
    cursor_refresh(cwin);
}

static void go_top() {

    cbuf->iline = 0;
    cbuf->cline = cbuf->head;
    win_scroll(cwin);
    win_render_num(cwin);
    win_render_status(cwin);
    cursor_refresh(cwin);
}

static void go_bottom() {
    cbuf->iline = cbuf->nlines - 1;
    cbuf->cline = cbuf->tail;
    win_scroll(cwin);
    win_render_num(cwin);
    win_render_status(cwin);
    cursor_refresh(cwin);
}

static void show_info() {
    mess_send("\"%s\" %d lines, --%d%%--", cbuf->name, cbuf->nlines,
              buffer_progress(cbuf));
}

static void swapchar() {
    Line *line = cbuf->cline;
    char *c = &line->content[cbuf->icol];
    *c = islower(*c) ? toupper(*c) : tolower(*c);
    win_render_lines(cwin);
}

static int nextrune(int inc) {
    int n;
    for (n = cbuf->icol + inc;
         n + inc >= 0 && (cbuf->cline->content[n] & 0xc0) == 0x80; n += inc)
        ;
    return n;
}

static void move_word_forward() {
    char *text = cbuf->cline->content;
    if (cbuf->icol > cbuf->cline->size) {
        cbuf->icol = 0;
        cbuf->iline++;
        cbuf->cline = cbuf->cline->next;
    } else {
        while (strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        while (!strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        cbuf->icol++;
    }

    win_render_lines(cwin);
}

static void move_end_word_forward() {
    char *text = cbuf->cline->content;
    cbuf->icol++;
    if (cbuf->icol > cbuf->cline->size) {
        cbuf->icol = 0;
        cbuf->iline++;
        cbuf->cline = cbuf->cline->next;
    } else {
        while (strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        while (!strchr(" ", text[cbuf->icol])) {
            cbuf->icol++;
        }
        cbuf->icol--;
    }

    win_render_lines(cwin);
}

static void move_word_backward() {
    char *text = cbuf->cline->content;
    if (cbuf->icol <= 0) {
        cbuf->iline--;
        cbuf->cline = cbuf->cline->prev;
        cbuf->icol = cbuf->cline->size;
    } else {
        while (strchr(" ", text[nextrune(-1)])) {
            cbuf->icol--;
        }
        while (!strchr(" ", text[nextrune(-1)])) {
            cbuf->icol--;
        }
    }
    win_render_lines(cwin);
}

static void find_char(int inc, char c) {
    int n = cbuf->icol + inc;
    while (n > 0 && n < cbuf->cline->size) {
        if (cbuf->cline->content[n] == c) {
            cbuf->icol = n;
            win_render_lines(cwin);
            return;
        }
        n += inc;
    }
}

void normal_mode(const int c) {
    switch (c) {
    // MOVE
    case KEY_LEFT:
    case 'h':
        cursor_left(cwin);
        break;
    case KEY_RIGHT:
    case 'l':
        cursor_right(cwin);
        break;
    case KEY_DOWN:
    case 'j':
        cursor_down(cwin);
        break;
    case KEY_UP:
    case 'k':
        cursor_up(cwin);
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
        move_screen(+cwin->wtext_lines / 2);
        break;
    case CTRL('u'):
        move_screen(-cwin->wtext_lines / 2);
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
        move_cursor(cwin->wtext_lines / 2 - 1);
        break;
    case 'L':
        move_cursor(cwin->wtext_lines - 1);
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
    case 'f':
        find_char(1, getch());
        break;
    case 'F':
        find_char(-1, getch());
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
        edit_del_str(cbuf->cline, 0, cbuf->cline->size);
        win_render_lines(cwin);
        mode_switch(MODE_INSERT);
        break;
    case 'J':
        join_line();
        break;
    case 'x':
        del_char();
        break;
    case 'I':
        move_start_line();
        mode_switch(MODE_INSERT);
        break;
    case 's':
        del_char();
        mode_switch(MODE_INSERT);
        break;
    case 'a':
        cursor_right(cwin);
        mode_switch(MODE_INSERT);
        break;
    case 'A':
        move_end_line();
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
