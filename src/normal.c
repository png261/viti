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
extern Buffer *curbuf;

void normal_mode(const int c) 
{
    Line *line = current_line(curwin);
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
        line = curbuf->nlines;
        win_scroll(curwin);
        cursor_refresh(curwin);
        break;
    case 'g':
        if (getch() == 'g') {
            line = 0;
            win_scroll(curwin);
            cursor_refresh(curwin);
        }
        break;
    case '0':
        curbuf->curcol = 0;
        cursor_refresh(curwin);
        break;
    case '$':
        curbuf->curcol = current_line(curwin)->size;
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
            line_remove(&curbuf->lines, line);
            win_render_lines(curwin);
            cursor_refresh(curwin);
        }
        break;
    case 'D':
        edit_del_end(line, curbuf->curcol);
        break;
    case 'C':
        edit_del_end(line, curbuf->curcol);
        mode_switch(MODE_INSERT);
        break;
    case 'S':
        edit_del_str(line, 0, current_line(curwin)->size);
        mode_switch(MODE_INSERT);
        break;
    case 'J':
        edit_join_line(line->next);
        break;
    case 'x':
        edit_del_char(line, curbuf->curcol);
        cursor_refresh(curwin);
        break;
    case 'I':
        curbuf->curcol = 0;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 's':
        edit_del_char(line, curbuf->curcol);
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'a':
        cursor_right(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'A':
        curbuf->curcol = current_line(curwin)->size;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'O':
        edit_add_line(line, "");
        curbuf->curcol = 0;
        cursor_refresh(curwin);
        mode_switch(MODE_INSERT);
        break;
    case 'o':
        line_insert(current_line(curwin), "", 1);
        curbuf->curcol = 0;
        curbuf->curline++;
        win_render_lines(curwin);
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
