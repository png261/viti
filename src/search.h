#ifndef VITI_SEARCH_H
#define VITI_SEARCH_H

#include "window.h"

#ifndef INCLUDE_TYPEDEF_WINDOW
#define INCLUDE_TYPEDEF_WINDOW
typedef struct window Win;
#endif 

typedef struct Pos{
    int x,y;
} Pos;

extern Pos * matched_list;
extern char *search_query;
extern int matched_index;
extern int matched_count;


void search_mode();
void search_next();
void search_prev();
void search(Win * win, const char * query);

#endif
