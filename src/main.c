#include <ncurses.h>
#include <stdlib.h>

#include "buffer.h"
#include "cursor.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "window.h"

#include "util.h"

extern struct Status status;
extern Win *cwin;

/* setup */
void ncursesSetup() {
    initscr();
    ESCDELAY = 10;
    start_color();
    raw();
    noecho();
    keypad(stdscr, TRUE);
}

void init() {
    int mess_height = 1;
    cwin = win_create(LINES - mess_height, COLS, 0, 0);
    cwin->buf = buffer_create(LINES - mess_height, COLS, 0, 0);

    status.win = newwin(mess_height, COLS, LINES - mess_height, 0);
    refresh();
}

/* main */
int main(int argc, char *argv[]) {
    ncursesSetup();
    init();

    if (argc >= 2) {
        file_open(argv[1], cwin->buf);
    }

    win_render(cwin);
    cursor_refresh(cwin);
    mode_switch(NORMAL);
    quit();
}
