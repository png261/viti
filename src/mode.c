#include "mode.h"

#include "buffer.h"
#include "command.h"
#include "insert.h"
#include "mess.h"
#include "normal.h"
#include "search.h"
#include "util.h"
#include <ncurses.h>

extern Win *cwin;

int cmode = NORMAL;

void loopKey(int mode, void (*callback)(int)) {
    while (1) {
        int c = wgetch(cwin->textarea);
        callback(c);
        if (cmode != mode) {
            break; 
        }
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
    case SEARCH:
        searchMode();
        break;
    }
}
