#ifndef VITI_HIGHLIGHT_H
#define VITI_HIGHTLIHGT_H

#include "window.h" 

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

void highlight_line(Win * win, Line *line, const char* query, int color_pair);
extern int is_highlight; 

#endif
