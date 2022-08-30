#ifndef VITI_HIGHLIGHT_H
#define VITI_HIGHLIGHT_H

#include "window.h" 

#include <stdbool.h>

#ifndef INCLUDE_TYPEDEF_WINDOW
#define INCLUDE_TYPEDEF_WINDOW
    typedef struct window Win;
#endif 

extern bool is_highlight; 

void highlight_line(WINDOW * win, int y, char *line, const char* query, int color_pair);

#endif // VITI_HIGHLIGHT_H
