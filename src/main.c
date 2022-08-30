#define _GNU_SOURCE

#include "buffer.h"
#include "color.h"
#include "cursor.h"
#include "fileio.h"
#include "message.h"
#include "mode.h"
#include "window.h"

#include "util.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern Message mess;
extern Win *curwin;
extern Buffer *curbuf;

static void ncurses_init() {
    // use newterm() instead of initscr() to enable pipes
    FILE *f = fopen("/dev/tty", "r+");
    SCREEN *screen = newterm(NULL, f, f);
    set_term(screen);

    set_escdelay(10);
    /* TODO: handle tab */
    set_tabsize(1);
    raw();
    noecho();
    keypad(stdscr, TRUE);
}

static void init() {
    ncurses_init();
    color_init();
    const int commandline = 1;
    curbuf = buffer_create();
    curwin = win_create(curbuf, LINES - commandline, COLS, 0, 0);
    mess.win = newwin(commandline, COLS, LINES - commandline, 0);
    refresh();
}

static void read_stdin() {
    // check if stdin not empty 
    // check it because getline will wait until it get some characters
    if (isatty(STDIN_FILENO)) { 
        return;
    }

    char *content = NULL;
    size_t linecap = 1;

    while (getline(&content, &linecap, stdin) != -1) {
        line_push(content, trim(content));
    }
}

static void create_first_line() {
    // create first line when open a blank file
    if (curbuf->nlines == 0) {
        line_push(NULL, 0);
        curbuf->nlines = 1;
    }
}

int main(int argc, char *argv[]) {
    init();

    if (argc >= 2) {
        file_open(argv[1], curbuf);
    } else {
        read_stdin();
    }
    create_first_line();

    curbuf->curline = curbuf->head;

    update_top_line(curwin);
    win_render(curwin);
    mode_switch(MODE_NORMAL);
}
