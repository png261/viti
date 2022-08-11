#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "insert.h"
#include "mode.h"
#include "util.h"

extern Buffer *cbuf;

void insertMode(int c) {
    switch (c) {
    /* move */
    case KEY_LEFT:
        cursor_left(cbuf);
        break;
    case KEY_RIGHT:
        cursor_right(cbuf);
        break;
    case KEY_DOWN:
        cursor_down(cbuf);
        break;
    case KEY_UP:
        cursor_up(cbuf);
        break;

    /* edit */
    case CTRL('c'):
    case '\x1b':
        mode_switch(NORMAL);
        break;
    case '\n':
        if (current_col(cbuf) == 0) {
            add_line(current_line(cbuf), "");
        } else {
            break_line(current_line(cbuf), current_col(cbuf));
            cbuf -> cur.x = 0;
            cbuf -> cur.y++;
            cursor_refresh(cbuf);
        }
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        del_char(current_line(cbuf), current_col(cbuf) - 1);
        cursor_left(cbuf);
        break;
    default:
        append_char(current_line(cbuf), current_col(cbuf), c);
        break;
    }
}
