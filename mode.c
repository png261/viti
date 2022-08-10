#include "mode.h"
#include "buffer.h"
#include "normal.h"
#include "insert.h"
#include "command.h"
#include <ncurses.h>

extern Buffer * cbuf; 

int cmode = NORMAL;

void loopKey(int mode, void (*callback)(int)) {
    while (1) {
        int c = wgetch(cbuf->win);
        callback(c);
        if (cmode != mode)
            break;
    }
}

void mode_switch(int mode) {
    cmode = mode;
    switch (cmode) {
    case NORMAL:
        loopKey(NORMAL, normalMode);
        break;
    case INSERT:
        loopKey(INSERT, insertMode);
        break;
    case COMMAND:
        commandMode();
        break;
    }
}
