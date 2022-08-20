#include "insert.h"

#include <ncurses.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "mode.h"
#include "util.h"
#include "window.h"

extern Win *cwin;

void insert_mode(const int c) {
    switch (c) {
    /* move */
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
        /* edit */
    case CTRL('c'):
    case '\x1b':
        mode_switch(NORMAL);
        break;
    case '\n':
        if (cwin->buf->col == 0) {
            edit_add_line(cwin->buf->line, "");
        } else {
            edit_break_line(cwin->buf->line, cwin->buf->col);
            cwin->buf->col = 0;
            cwin->buf->line++;
            cursor_refresh(cwin);
        }
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        edit_del_char(cwin->buf->line, cwin->buf->col - 1);
        cursor_left(cwin);
        break;
    default:
        edit_append_char(cwin->buf->line, cwin->buf->col, c);
        break;
    }
}
