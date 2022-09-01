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
extern Win *cwin;
extern Buffer *cbuf;

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
    cbuf = buffer_create();
    cwin = win_create(cbuf, LINES - commandline, COLS, 0, 0);
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
    if (cbuf->nlines == 0) {
        line_push(NULL, 0);
        cbuf->nlines = 1;
    }
}

int main(int argc, char *argv[]) {
    init();

    if (argc >= 2) {
        file_open(argv[1], cbuf);
    } else {
        read_stdin();
    }
    create_first_line();

    cbuf->cline = cbuf->head;

    update_tline(cwin);
    win_render(cwin);
    mode_switch(MODE_NORMAL);
}
