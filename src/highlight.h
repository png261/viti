#ifndef VITI_HIGHLIGHT_H
#define VITI_HIGHTLIHGT_H

#include "window.h" 

#include <stdbool.h>

#ifndef INCLUDE_TYPEDEF_WINDOW
#define INCLUDE_TYPEDEF_WINDOW
typedef struct window Win;
#endif 

extern bool is_highlight; 

void highlight_line(WINDOW * win, char *line, const char* query, int color_pair, int y);

#endif
