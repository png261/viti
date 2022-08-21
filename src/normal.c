#include "normal.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "fileio.h"
#include "message.h"
#include "mode.h"
#include "search.h"
#include "util.h"
#include "window.h"


extern Win *curwin;

void normal_mode(const int c) 
{
    switch (c) {
    /* move */
    case KEY_LEFT:
    case 'h':
        cursor_left(curwin);
        break;
    case KEY_RIGHT:
    case 'l':
        cursor_right(curwin);
        break;
    case KEY_DOWN:
    case 'j':
        cursor_down(curwin);
        break;
    case KEY_UP:
    case 'k':
        cursor_up(curwin);
        break;
    case 'G':
        curwin->buf->line = curwin->buf->file.lines;
        win_scroll(curwin);
        cursor_refresh(curwin);
        break;
    case 'g':
        if (getch() == 'g') {
            curwin->buf->line = 0;
            win_scroll(curwin);
            cursor_refresh(curwin);
        }
        break;
    case '0':
        curwin->buf->col = 0;
        cursor_refresh(curwin);
        break;
    case '$':
        curwin->buf->col = current_row(curwin)->size;
        cursor_refresh(curwin);
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
            edit_del_line(current_line(curwin));
            win_render_rows(curwin);
            cursor_refresh(curwin);
        }
        break;
    case 'D':
        edit_del_end(current_line(curwin), current_col(curwin));
        break;
    case 'C':
        edit_del_end(current_line(curwin), current_col(curwin));
        mode_switch(MODE_INSERT);
        break;
    case 'S':
        edit_del_str(current_line(curwin), 0, current_row(curwin)->size);
        mode_switch(MODE_INSERT);
        break;
    case 'J':
        edit_join_line(current_line(curwin) + 1);
        break;
    case 'x':
        edit_del_char(current_line(curwin), current_col(curwin));
        cursor_refresh(curwin);
        break;
    case 'I':
        curwin->buf->col = 0;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 's':
        edit_del_char(current_line(curwin), current_col(curwin));
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'a':
        cursor_right(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'A':
        curwin->buf->col = current_row(curwin)->size;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'O':
        edit_add_line(current_line(curwin), "");
        curwin->buf->col = 0;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'o':
        edit_add_line(current_line(curwin) + 1, "");
        curwin->buf->col = 0;
        curwin->buf->line++;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'i':
        mode_switch(MODE_INSERT);
        break;
    case ':':
        mode_switch(MODE_COMMAND);
        break;
    case '/':
        mode_switch(MODE_SEARCH);
        break;
    }
}
