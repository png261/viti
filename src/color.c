#include "color.h"

#include <ncurses.h>


void color_init() 
{
    start_color();
    init_color(COLOR_STATUSLINE_BG, 200, 200, 200);
    init_pair(PAIR_HIGHLIGHT, COLOR_BLACK, COLOR_GREEN);
    init_pair(PAIR_NUMBERCOL, COLOR_YELLOW, COLOR_BLACK);
    init_pair(PAIR_STATUSLINE, COLOR_WHITE, COLOR_STATUSLINE_BG);
}
