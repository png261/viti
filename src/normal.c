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
    case 'G':
        go_to_line(cbuf, cbuf->file.lines);
        break;
    case 'g':
        if (getch() == 'g') {
            go_to_line(cbuf, 0);
        }
        break;
    /* edit */
    case 'd':
        if (getch() == 'd') {
            del_line(current_line(cbuf));
            buffer_render_rows(cbuf);
            cursor_refresh(cbuf);
        }
        break;
    case 'D':
        del_end(current_line(cbuf), current_col(cbuf));
        break;
    case 'C':
        del_end(current_line(cbuf), current_col(cbuf));
        mode_switch(INSERT);
        break;
    case 'S':
        del_str(current_line(cbuf), 0, current_row(cbuf)->size);
        mode_switch(INSERT);
        break;
    case 'J':
        join_line(current_line(cbuf) + 1);
        break;
    case 'x':
        del_char(current_line(cbuf), current_col(cbuf));
        cursor_refresh(cbuf);
        break;
    case 'I':
        cbuf->cur.x = 0;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 's':
        del_char(current_line(cbuf), current_col(cbuf));
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'a':
        cursor_right(cbuf);
        mode_switch(INSERT);
        break;
    case 'A':
        cbuf->cur.x = current_row(cbuf)->size;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'O':
        add_line(current_line(cbuf), "");
        cbuf->cur.x = 0;
        cursor_refresh(cbuf);
        mode_switch(INSERT);
        break;
    case 'o':
        add_line(current_line(cbuf) + 1, "");
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
