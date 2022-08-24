#include "insert.h"

#include <ncurses.h>

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "mode.h"
#include "util.h"
#include "window.h"
#include "message.h"


extern Win *curwin;
extern Buffer *curbuf;

void insert_mode(const int c) {
    Line *line = current_line(curwin);

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
        if (curbuf->curcol == 0) {
            edit_add_line(line, "");
        } else {
            edit_break_line(line, curbuf->curcol);
            curbuf->curcol = 0;
            curbuf->curline++;
            cursor_refresh(curwin);
        }
        break;
    case CTRL('h'):
    case KEY_BACKSPACE:
        edit_del_char(line, curbuf->curcol - 1);
        break;
    default:
        edit_append_char(line, curbuf->curcol, c);
        break;
    }
}
