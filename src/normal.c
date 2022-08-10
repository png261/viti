#include "buffer.h"
#include "edit.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "normal.h"

extern Buffer *cbuf;

void normalMode(int c) {
    switch (c) {
    /* move */
    case 'h':
        cursor_left(cbuf);
        break;
    case 'l':
        cursor_right(cbuf);
        break;
    case 'j':
        cursor_down(cbuf);
        break;
    case 'k':
        cursor_up(cbuf);
        break;
    /* edit */
    case 'D':
        del_end(cbuf->view.line, cbuf->view.col);
        break;
    case 'C':
        del_end(cbuf->view.line, cbuf->view.col);
        mode_switch(INSERT);
        break;
    case 'S':
        del_str(cbuf->view.line, 0, cbuf->rows[cbuf->view.line].size);
        mode_switch(INSERT);
        break;
    case 'J':
        join_line(cbuf->view.line + 1);
        break;
    case 'x':
        del_char(cbuf->view.line, cbuf->view.col);
        cursor_refresh(cbuf);
        break;
    case 'I':
        cbuf->cur.x = 0;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 's':
        del_char(cbuf->view.line, cbuf->view.col);
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'a':
        cursor_right(cbuf);
        mode_switch(INSERT);
        break;
    case 'A':
        cbuf->cur.x = cbuf->rows[cbuf->view.line].size;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'O':
        add_line(cbuf->view.line, "");
        cbuf->cur.x = 0;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'o':
        add_line(cbuf->view.line + 1, "");
        cbuf->cur.x = 0;
        cbuf->cur.y++;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'i':
        mode_switch(INSERT);
        break;
    case ':':
        mode_switch(COMMAND);
        break;
    }
}
