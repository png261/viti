#include "buffer.h"
#include "edit.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "normal.h"

extern Buffer *cbuf;

void normalMode(int c) {
    switch (c) {
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
    case 'i':
        mode_switch(INSERT);
        break;
    case ':':
        mode_switch(COMMAND);
        break;
    }
    mess_send(cbuf->rows[cbuf->view.line].content);
}
