#include "insert.h"

#include <ncurses.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "mode.h"
#include "util.h"
#include "window.h"

extern Win *cwin;

void insertMode(int c) {
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
        if (current_col(cwin) == 0) {
            add_line(current_line(cwin), "");
        } else {
            break_line(current_line(cwin), current_col(cwin));
            cwin->buf->col = 0;
            cwin->buf->line++;
            cursor_refresh(cwin);
        }
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        del_char(current_line(cwin), current_col(cwin) - 1);
        cursor_left(cwin);
        break;
    default:
        append_char(current_line(cwin), current_col(cwin), c);
        break;
    }
}
