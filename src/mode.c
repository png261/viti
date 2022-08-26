#include "mode.h"

#include "buffer.h"
#include "command.h"
#include "window.h"
#include "insert.h"
#include "message.h"
#include "normal.h"
#include "search.h"
#include "util.h"

#include <ncurses.h>


extern Win *curwin;
enum MODE State = MODE_NORMAL;


void loopKey(enum MODE mode, void (*callback)(const int)) 
{
    while (1) {
        int c = getch();
        if(c == KEY_RESIZE){
            /* TODO: resize all windows when have mutil */
            win_resize(curwin, LINES - 1, COLS);
            mess_resize();
            refresh();
            win_render(curwin);
            continue;
        }

        callback(c);
        if (State != mode) {
            break; 
        }
    }
}


void mode_switch(enum MODE mode) 
{
    State = mode;
    switch (State) {
        case MODE_NORMAL:
            loopKey(MODE_NORMAL, normal_mode);
            break;
        case MODE_INSERT:
            loopKey(MODE_INSERT, insert_mode);
            break;
        case MODE_COMMAND:
            command_mode();
            break;
        case MODE_SEARCH:
            search_mode();
            break;
    }
}
