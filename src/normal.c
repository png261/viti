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
#include "key.h"

#include <ctype.h> 


extern Win *curwin;
extern Buffer *curbuf;


void nor_new_line_above()
{
    line_insert_before(&curbuf->lines, current_line(curwin), NULL, 0);
    curbuf->nlines++;
    win_render_lines(curwin);
    curbuf->curcol = 0;
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}


void nor_new_line_below()
{
    Line *line = current_line(curwin);
    line_insert_after(line, NULL, 0);
    curbuf->nlines++;
    win_render_lines(curwin);
    curbuf->curcol = 0;
    curbuf->curline++;
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}


void nor_join_line()
{
    Line *line = current_line(curwin);
    curbuf->curcol = line->size;
    edit_join_line(line); 
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


void nor_del_end() 
{
    Line * line = current_line(curwin);
    if(line == NULL) {
        return;
    }
    edit_del_str(line, curbuf->curcol, line->size - curbuf->curcol);
    win_render_lines(curwin);
}


void nor_del_line()
{
    line_remove(&curbuf->lines, current_line(curwin));
    curbuf->nlines--;
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


void nor_replace_char()
{
    char c = getch();
    /* TODO: fix special character */
    if(c == ESC) {
        return;
    }
    Line *line = current_line(curwin);
    line->content[curbuf->curcol] = c;
    win_render_lines(curwin);
}


void nor_del_char()
{
    edit_del_char(current_line(curwin), curbuf->curcol);
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


void nor_move_end_line()
{
    curbuf->curcol = current_line(curwin)->size;
    cursor_refresh(curwin);
}


void nor_move_start_line()
{
    curbuf->curcol = 0;
    cursor_refresh(curwin);
}

void nor_move_screen(int lines)
{
    curwin->view.yoff += lines;
    curbuf->curline+= lines;
    LIMIT(curwin->view.yoff, 0, curbuf->nlines - 1);
    LIMIT(curbuf->curline, 0, curbuf->nlines - 1);
    mess_send("%d", curbuf->curline);
    win_render_lines(curwin);
}

void nor_move_cursor(int lines)
{
    curbuf->curline = curwin->view.yoff + lines;
    cursor_refresh(curwin);
}

void nor_go_top()
{

    curbuf->curline = 0;
    win_scroll(curwin);
    cursor_refresh(curwin);
}

void nor_go_bottom()
{
    curbuf->curline = curbuf->nlines;
    win_scroll(curwin);
    cursor_refresh(curwin);
}

void nor_show_info()
{
   mess_send("\"%s\" %d lines, --%d%%--", curbuf->file.name, curbuf->nlines, buffer_progress(curwin)); 
}

void nor_swapchar()
{
    Line *line = current_line(curwin);
    char *c =  &line->content[curbuf->curcol];
    *c = islower(*c) ? toupper(*c) : tolower(*c);
    win_render_lines(curwin);
}

void normal_mode(const int c) 
{
    switch (c) {
        // MOVE
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
            nor_go_bottom();
            break;
        case 'g':
            if (getch() == 'g') {
                nor_go_top();
            }
            break;
        case CTRL('d'):
            nor_move_screen(+curwin->textarea_lines / 2);
            break;
        case CTRL('u'):
            nor_move_screen(-curwin->textarea_lines / 2);
            break;
        case CTRL('e'):
            nor_move_screen(-1);
            break;
        case CTRL('y'):
            nor_move_screen(+1);
            break;
        case CTRL('g'):
            nor_show_info();
            break;
        case CTRL('o'):
            /* TODO move backword jump history */
            break;
        case CTRL('i'):
            /* TODO move forward jump history */
            break;
        case 'H':
            nor_move_cursor(0);
            break;
        case 'M':
            nor_move_cursor(curwin->textarea_lines / 2 - 1);
            break;
        case 'L':
            nor_move_cursor(curwin->textarea_lines - 1);
            break;
        case 'w':
            /* TODO move word by word */
            break;
        case 'e':
            /* TODO move end of word */
            break;
        case '0':
            nor_move_start_line();
            break;
        case '$':
            nor_move_end_line();
            break;


        /* TODO:search */
        /* case 'n': */
        /*     search_next(); */
        /*     break; */
        /* case 'N': */
        /*     search_prev(); */
        /*     break; */

        // EDIT
        case 'd':
            if (getch() == 'd') {
                nor_del_line();
            }
            break;
        case 'D':
            nor_del_end(); 
            break;
        case 'C':
            nor_del_end();
            mode_switch(MODE_INSERT);
            break;
        case 'r':
            nor_replace_char();
            break;
        case 'S':
            edit_del_str(current_line(curwin), 0, current_line(curwin)->size);
            mode_switch(MODE_INSERT);
            break;
        case 'J':
            nor_join_line();
            break;
        case 'x':
            nor_del_char();
            break;
        case 'I':
            nor_move_end_line();
            mode_switch(MODE_INSERT);
            break;
        case 's':
            edit_del_char(current_line(curwin), curbuf->curcol);
            cursor_refresh(curwin);
            mode_switch(MODE_INSERT);
            break;
        case 'a':
            cursor_right(curwin);
            mode_switch(MODE_INSERT);
            break;
        case 'A':
            nor_move_start_line();
            mode_switch(MODE_INSERT);
            break;
        case 'O':
            nor_new_line_above();
            break;
        case 'o':
            nor_new_line_below();
            break;
        case '~':
            nor_swapchar();
            break;

        // MODE
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
