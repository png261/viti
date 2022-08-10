#include <ncurses.h>
#include <stdlib.h>

#include "buffer.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "util.h"

extern struct Status status;
extern Buffer *cbuf;
extern Buffer *Bufs;

/* setup */
void ncursesSetup() {
    initscr();
    start_color();
    raw();
    keypad(stdscr, TRUE);
    noecho();
}

void init() {
    int mess_height = 1;
    Bufs = malloc(sizeof(Buffer));
    Bufs[0] = buffer_create(LINES - mess_height, COLS, 0, 0);
    cbuf = &Bufs[0];
    status.win = newwin(mess_height, COLS, LINES - mess_height, 0);
    refresh();
}

/* main */
int main(int argc, char *argv[]) {
    ncursesSetup();
    init();

    if (argc >= 2) {
        file_open(argv[1], cbuf);
    }

    buffer_render(cbuf);
    cursor_refresh(cbuf);
    mode_switch(NORMAL);
    quit();
}
