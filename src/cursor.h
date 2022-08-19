#ifndef CURSOR_H
#define CURSOR_H

#include "window.h"

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 


void cursor_refresh(Win * win);
void cursor_left(Win * win);
void cursor_right(Win * win);
void cursor_up(Win * win);
void cursor_down(Win * win);

void handleScroll(Win *win);
#endif

