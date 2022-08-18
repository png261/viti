#ifndef COLOR_H
#define COLOR_H

#define COLOR_BG COLOR_BLACK

enum pairs_color {
    PAIR_NUMBERCOL = 1,
    PAIR_STATUSLINE,
    PAIR_HIGHLIGHT,
    COLOR_STATUSLINE_BG,
};

void color_init();
#endif

