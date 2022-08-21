#include "insert.h"

#include <ncurses.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "mode.h"
#include "util.h"
#include "window.h"


extern Win *curwin;

void insert_mode(const int c) {
    switch (c) {
    /* move */
    case KEY_LEFT:
        cursor_left(curwin);
        break;
    case KEY_RIGHT:
        cursor_right(curwin);
        break;
    case KEY_DOWN:
        cursor_down(curwin);
        break;
    case KEY_UP:
        cursor_up(curwin);
        break;
        /* edit */
    case CTRL('c'):
    case '\x1b':
        mode_switch(MODE_NORMAL);
        break;
    case '\n':
        if (curwin->buf->col == 0) {
            edit_add_line(curwin->buf->line, "");
        } else {
            edit_break_line(curwin->buf->line, curwin->buf->col);
            curwin->buf->col = 0;
            curwin->buf->line++;
            cursor_refresh(curwin);
        }
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        edit_del_char(curwin->buf->line, curwin->buf->col - 1);
        break;
    default:
        edit_append_char(curwin->buf->line, curwin->buf->col, c);
        break;
    }
}
