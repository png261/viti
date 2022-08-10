#include "buffer.h"
#include "edit.h"
#include "insert.h"
#include "mode.h"

extern Buffer *cbuf;

void insertMode(int c) {
    switch (c) {
    case '\x1b':
        mode_switch(NORMAL);
        break;
    case '\n':
        add_line(cbuf->view.line, "");
        break;
    case KEY_BACKSPACE:
        del_char(cbuf->view.line, cbuf->view.col - 1);
        break;
    default:
        append_char(cbuf->view.line, cbuf->view.col, c);
        break;
    }
}
