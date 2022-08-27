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

static void nor_new_line_above()
{
    line_insert_before(&curbuf->head, curbuf->curline, NULL, 0);
    curbuf->nlines++;
    curbuf->icol = 0;
    curbuf->curline = curbuf->curline->prev;
    update_top_line(curwin);
    win_render_lines(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}


static void nor_new_line_below()
{
    Line *line = curbuf->curline;
    line_insert_after(line, &curbuf->tail, NULL, 0);
    curbuf->nlines++;
    curbuf->icol = 0;
    curbuf->iline++;
    curbuf->curline = curbuf->curline->next;
    win_render_lines(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_INSERT);
}


static void nor_join_line()
{
    Line *line = curbuf->curline;
    curbuf->icol = line->size;
    edit_join_line(line); 
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


static void nor_del_end() 
{
    Line * line = curbuf->curline;
    if(line == NULL) {
        return;
    }
    edit_del_str(line, curbuf->icol, line->size - curbuf->icol);
    win_render_lines(curwin);
}


static void nor_del_line()
{
    line_remove(&curbuf->head, &curbuf->tail, curbuf->curline);
    update_top_line(curwin);
    curbuf->curline = curbuf->curline->next;
    curbuf->nlines--;
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


static void nor_replace_char()
{
    char c = getch();
    /* TODO: fix special character */
    if(c == ESC) {
        return;
    }
    Line *line = curbuf->curline;
    line->content[curbuf->icol] = c;
    win_render_lines(curwin);
}


static void nor_del_char()
{
    edit_del_char(curbuf->curline, curbuf->icol);
    win_render_lines(curwin);
    cursor_refresh(curwin);
}


static void nor_move_end_line()
{
    curbuf->icol = curbuf->curline->size;
    cursor_refresh(curwin);
}


static void nor_move_start_line()
{
    curbuf->icol = 0;
    cursor_refresh(curwin);
}

static void nor_move_screen(int lines)
{
    /* TODO: fix */
    curwin->yoff += lines;
    curbuf->iline+= lines;
    LIMIT(curwin->yoff, 0, curbuf->nlines - 1);
    LIMIT(curbuf->iline, 0, curbuf->nlines - 1);
    win_render_lines(curwin);
}

static void nor_move_cursor(int lines)
{
    curbuf->iline = curwin->yoff + lines;
    cursor_refresh(curwin);
}

static void nor_go_top()
{

    curbuf->iline = 0;
    curbuf->curline = curbuf->head;
    win_scroll(curwin);
    win_render_numbercol(curwin);
    win_render_statusline(curwin);
    cursor_refresh(curwin);
}

static void nor_go_bottom()
{
    curbuf->iline = curbuf->nlines - 1;
    curbuf->curline = curbuf->tail;
    win_scroll(curwin);
    win_render_numbercol(curwin);
    win_render_statusline(curwin);
    cursor_refresh(curwin);
}

static void nor_show_info()
{
   mess_send("\"%s\" %d lines, --%d%%--", curbuf->name, curbuf->nlines, buffer_progress(curbuf)); 
}

static void nor_swapchar()
{
    Line *line = curbuf->curline;
    char *c =  &line->content[curbuf->icol];
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
            nor_move_screen(+curwin->wtext_lines / 2);
            break;
        case CTRL('u'):
            nor_move_screen(-curwin->wtext_lines / 2);
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
            nor_move_cursor(curwin->wtext_lines / 2 - 1);
            break;
        case 'L':
            nor_move_cursor(curwin->wtext_lines - 1);
            break;
        case 'w':
            /* TODO move word by word */
            break;
        case 'e':
            /* TODO move end of word */
            break;
        case 'Z':
            if(getch() == 'Q') {
                viti_quit();
            }
            break;
        case '0':
            nor_move_start_line();
            break;
        case '$':
            nor_move_end_line();
            break;

        // SEARCH
        case 'n':
            search_next();
            break;
        case 'N':
            search_prev();
            break;

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
            edit_del_str(curbuf->curline, 0, curbuf->curline->size);
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
            edit_del_char(curbuf->curline, curbuf->icol);
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
