#ifndef HIGHLIGHT_H
#define HIGHTLIHGT_H
#include "window.h" 

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

void highlight_row(Win * win, int line, char* query, int color_pair);
#endif
