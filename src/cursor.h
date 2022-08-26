#ifndef VITI_CURSOR_H
#define CURSOR_H

#include "window.h"


#ifndef INCLUDE_TYPEDEF_WINDOW
#define INCLUDE_TYPEDEF_WINDOW
typedef struct window Win;
#endif 


void cursor_refresh(Win * win);
void cursor_left(Win * win);
void cursor_right(Win * win);
void cursor_up(Win * win);
void cursor_down(Win * win);

#endif

