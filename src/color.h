#ifndef VITI_COLOR_H
#define VITI_COLOR_H

#define COLOR_BG COLOR_BLACK

enum PAIR_COLOR 
{
    PAIR_NUMBERCOL = 1,
    PAIR_STATUSLINE,
    PAIR_HIGHLIGHT,
    COLOR_STATUSLINE_BG,
};

void color_init();

#endif

