#include "insert.h"

#include "buffer.h"
#include "cursor.h"
#include "edit.h"
#include "key.h"
#include "mode.h"
#include "util.h"
#include "window.h"
#include "message.h"

#include <ncurses.h>
#include <string.h>


extern Win *curwin;
extern Buffer *curbuf;


void ins_del_to_start()
{
    edit_del_str(current_line(curwin), 0, curbuf->curcol);
    win_render_lines(curwin);
    curbuf->curcol = 0;
    cursor_refresh(curwin);
}


void ins_put_char(char c)
{
    edit_append_char(current_line(curwin), curbuf->curcol, c);
    win_render_lines(curwin);
    curbuf->curcol++;
    cursor_refresh(curwin);
}


void ins_del_char()
{
    Line *line = current_line(curwin);
    if (curbuf->curcol == 0) {
        curbuf->curcol = line->prev->size;
        edit_join_line(line->prev);
        curbuf->curline--;
    } else {
        edit_del_char(line, curbuf->curcol - 1); 
    }
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


void ins_break_line(Line * line) 
{
    char *str = edit_del_str(line, curbuf->curcol, line->size - curbuf->curcol + 1);
    line_insert_after(line, str, strlen(str));

    win_render_lines(curwin);
    curbuf->curcol = 0;
    curbuf->curline++;
    cursor_refresh(curwin);
}


void ins_add_line_before(Line * line)
{
    line_insert_before(&curbuf->lines, line, "", 0);
    win_render_lines(curwin);
    curbuf->curcol = 0;
    curbuf->curline++;
    cursor_refresh(curwin);
}


void ins_add_line_after(Line * line)
{
    line_insert_after(line, "", 0);
    win_render_lines(curwin);
    curbuf->curcol = 0;
    curbuf->curline++;
    cursor_refresh(curwin);
}


void ins_enter()
{
    Line * line = current_line(curwin);
    if (curbuf->curcol >= line->size) {
        ins_add_line_after(line);
        return;
    } 
    if (curbuf->curcol == 0) {
        ins_add_line_before(line);
        return;
    } 
    ins_break_line(line);
}


void insert_mode(const int c) 
{
    switch (c) {
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
        case CTRL('w'):
            /* TODO del one word */
            break;
        case CTRL('u'):
            ins_del_to_start();
            break;
        case CTRL('c'):
        case ESC:
            mode_switch(MODE_NORMAL);
            break;
        case '\n':
            ins_enter();
            break;
        case CTRL('h'):
        case KEY_BACKSPACE:
            ins_del_char();
            break;
        default:
            ins_put_char(c);
            break;
    }
}
