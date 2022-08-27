#include "buffer.h"
#include "color.h"
#include "cursor.h"
#include "fileio.h"
#include "message.h"
#include "mode.h"
#include "window.h"

#include "util.h"

#include <ncurses.h>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>


extern Message mess;
extern Win *curwin;
extern Buffer *curbuf;


static void ncurses_init() 
{
    initscr();
    ESCDELAY = 10;
    raw();
    noecho();
    keypad(stdscr, TRUE);
}


static void init() 
{
    ncurses_init();
    color_init();
    const int mess_height = 1;
    curbuf = buffer_create();
    curwin = win_create(curbuf, LINES - mess_height, COLS, 0, 0);
    mess.win = newwin(mess_height, COLS, LINES - mess_height, 0);
    refresh();
}


void check_line()
{
    if(curbuf->nlines == 0) {
        line_push(&curbuf->head, &curbuf->tail, NULL, 0);
        curbuf->nlines = 1;
    }
}

int main(int argc, char *argv[]) 
{
    init();

    if (argc >= 2) {
        file_open(argv[1],curbuf);
    }

    check_line();
    update_top_line(curwin);
    win_render(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_NORMAL);
    viti_quit();
}
