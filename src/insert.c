// insert.c: handle insert mode

#include "insert.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "key.h"
#include "mode.h"
#include "util.h"
#include "window.h"

#include <ncurses.h>
#include <string.h>

extern Win *cwin;
extern Buffer *cbuf;

static void ins_del_to_start() {
    edit_del_str(cbuf->cline, 0, cbuf->icol);
    win_render_lines(cwin);
    cbuf->icol = 0;
    cursor_refresh(cwin);
}

static void ins_put_char(char c) {
    edit_append_char(cbuf->cline, cbuf->icol, c);
    cbuf->icol++;
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void ins_join_line() {
    if (cbuf->iline == 0) {
        return;
    }
    cbuf->icol = cbuf->cline->prev->size;
    edit_join_line(cbuf->cline->prev);
    cbuf->cline = cbuf->cline->prev;
    cbuf->iline--;
}

static void ins_del_char() {
    if (cbuf->icol == 0) {
        ins_join_line();
    } else {
        edit_del_char(cbuf->cline, cbuf->icol - 1);
        cbuf->icol--;
    }
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void ins_break_line() {
    Line *line = cbuf->cline;
    int len = line->size - cbuf->icol;
    char *del_str = edit_substr(line->content, cbuf->icol, len);
    edit_del_str(line, cbuf->icol, len);
    line_insert_after(line, del_str, len);
    cbuf->cline = line->next;

    cbuf->icol = 0;
    cbuf->iline++;
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void ins_add_line_after() {
    line_insert_after(cbuf->cline, NULL, 0);
    cbuf->icol = 0;
    cbuf->iline++;
    cbuf->cline = cbuf->cline->next;
    win_render_lines(cwin);
    cursor_refresh(cwin);
}

static void ins_enter() {
    if (cbuf->icol >= cbuf->cline->size) {
        ins_add_line_after();
        return;
    }
    ins_break_line();
}

void insert_mode(const int c) {
    switch (c) {
    case KEY_LEFT:
        cursor_left(cwin);
        break;
    case KEY_RIGHT:
        cursor_right(cwin);
        break;
    case KEY_DOWN:
        cursor_down(cwin);
        break;
    case KEY_UP:
        cursor_up(cwin);
        break;
    case CTRL('w'):
        /* TODO del one word */
        break;
    case CTRL('u'):
        ins_del_to_start();
        break;
    case CTRL('c'):
    case ESC:
        mode_switch(MODE_NORMAL);
        break;
    case '\n':
        ins_enter();
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        ins_del_char();
        break;
    default:
        ins_put_char(c);
        break;
    }
}
