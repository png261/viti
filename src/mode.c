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
static int State = MODE_NORMAL;

void resize()
{
    win_resize(curwin, LINES - 1, COLS);
    mess_resize();
    refresh();
    win_render(curwin);
}

static void mode_loop(int mode, void (*handle_key)(const int)) 
{
    do {
        const int c = getch();
        if(c == KEY_RESIZE) {
            /* TODO: resize all windows when have mutil */
            resize();
            continue;
        }

        handle_key(c);
    }
    while(State == mode);
}


void mode_switch(int mode) 
{
    switch (State = mode) {
        case MODE_NORMAL:
            mode_loop(MODE_NORMAL, normal_mode);
            break;
        case MODE_INSERT:
            mode_loop(MODE_INSERT, insert_mode);
            break;
        case MODE_COMMAND:
            command_mode();
            break;
        case MODE_SEARCH:
            search_mode();
            break;
    }
}
