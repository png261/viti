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


static void ins_del_to_start()
{
    edit_del_str(curbuf->curline, 0, curbuf->icol);
    win_render_lines(curwin);
    curbuf->icol = 0;
    cursor_refresh(curwin);
}


static void ins_put_char(char c)
{
    edit_append_char(curbuf->curline, curbuf->icol, c);
    win_render_lines(curwin);
    curbuf->icol++;
    cursor_refresh(curwin);
}


static void ins_del_char()
{
    Line *line = curbuf->curline;
    if (curbuf->icol == 0) {
        curbuf->icol = line->prev->size;
        edit_join_line(line->prev);
        curbuf->iline--;
    } else {
        edit_del_char(line, curbuf->icol - 1); 
    }
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


static void ins_break_line(Line * line) 
{
    char *str = edit_del_str(line, curbuf->icol, line->size - curbuf->icol + 1);
    line_insert_after(line, &curbuf->tail, str, strlen(str));

    win_render_lines(curwin);
    curbuf->icol = 0;
    curbuf->iline++;
    cursor_refresh(curwin);
}


static void ins_add_line_before(Line * line)
{
    line_insert_before(&curbuf->head, line, NULL, 0);
    curbuf->nlines++;
    win_render_lines(curwin);
    curbuf->icol = 0;
    curbuf->iline++;
    cursor_refresh(curwin);
}


static void ins_add_line_after(Line * line)
{
    line_insert_after(line, &curbuf->tail, NULL, 0);
    curbuf->nlines++;
    win_render_lines(curwin);
    curbuf->icol = 0;
    curbuf->iline++;
    cursor_refresh(curwin);
}


static void ins_enter()
{
    Line * line = curbuf->curline;
    if (curbuf->icol >= line->size) {
        ins_add_line_after(line);
        return;
    } 
    if (curbuf->icol == 0) {
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
