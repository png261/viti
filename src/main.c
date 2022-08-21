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


extern Message mess;
extern Win *curwin;

/* setup */
void ncurses_init() 
{
    initscr();
    ESCDELAY = 10;
    raw();
    noecho();
    keypad(stdscr, TRUE);
}

void init() 
{
    ncurses_init();
    color_init();
    const int mess_height = 1;
    curwin = win_create(NULL, LINES - mess_height, COLS, 0, 0);
    mess.win = newwin(mess_height, COLS, LINES - mess_height, 0);
    refresh();
}

/* main */
int main(int argc, char *argv[]) 
{
    init();

    if (argc >= 2) {
        file_open(argv[1], curwin->buf);
    }

    win_render(curwin);
    cursor_refresh(curwin);
    mode_switch(MODE_NORMAL);
    quit();
}
