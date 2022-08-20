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

void loopKey(int mode, void (*callback)(const int)) 
{
    while (1) {
        const int c = wgetch(cwin->textarea);
        callback(c);
        if (cmode != mode) {
            break; 
        }
    }
}

void mode_switch(const int mode) 
{
    cmode = mode;
    switch (cmode) {
    case NORMAL:
        loopKey(NORMAL, normal_mode);
        break;
    case INSERT:
        loopKey(INSERT, insert_mode);
        break;
    case COMMAND:
        command_mode();
        break;
    case SEARCH:
        search_mode();
        break;
    }
}
