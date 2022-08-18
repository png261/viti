#include "normal.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include "window.h"

extern Win *cwin;
extern MatchedList *matched_list;

void normalMode(int c) {
    switch (c) {
    /* move */
    case 'h':
        cursor_left(cwin);
        break;
    case 'l':
        cursor_right(cwin);
        break;
    case 'j':
        cursor_down(cwin);
        break;
    case 'k':
        cursor_up(cwin);
        break;
    /* search */
    case 'n':
        search_next();
        break;
    case 'N':
        search_prev();
        break;
    /* edit */
    case 'd':
        if (getch() == 'd') {
            del_line(current_line(cwin));
            win_render_rows(cwin);
            cursor_refresh(cwin);
        }
        break;
    case 'D':
        del_end(current_line(cwin), current_col(cwin));
        break;
    case 'C':
        del_end(current_line(cwin), current_col(cwin));
        mode_switch(INSERT);
        break;
    case 'S':
        del_str(current_line(cwin), 0, current_row(cwin)->size);
        mode_switch(INSERT);
        break;
    case 'J':
        join_line(current_line(cwin) + 1);
        break;
    case 'x':
        del_char(current_line(cwin), current_col(cwin));
        cursor_refresh(cwin);
        break;
    case 'I':
        cwin->buf->col = 0;
        cursor_refresh(cwin);
        mode_switch(INSERT);
        break;
    case 's':
        del_char(current_line(cwin), current_col(cwin));
        cursor_refresh(cwin);
        mode_switch(INSERT);
        break;
    case 'a':
        cursor_right(cwin);
        mode_switch(INSERT);
        break;
    case 'A':
        cwin->buf->col = current_row(cwin)->size;
        cursor_refresh(cwin);
        mode_switch(INSERT);
        break;
    case 'O':
        add_line(current_line(cwin), "");
        cwin->buf->col = 0;
        cursor_refresh(cwin);
        mode_switch(INSERT);
        break;
    case 'o':
        add_line(current_line(cwin) + 1, "");
        cwin->buf->col = 0;
        cwin->buf->line++;
        cursor_refresh(cwin);
        mode_switch(INSERT);
        break;
    case 'i':
        mode_switch(INSERT);
        break;
    case ':':
        mode_switch(COMMAND);
        break;
    case '/':
        mode_switch(SEARCH);
        break;
    }
}
