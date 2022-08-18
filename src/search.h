#ifndef SEARCH_H
#define SEARCH_H
#include "window.h"

#ifndef TYPEDEF_WIN
#define TYPEDEF_WIN
typedef struct window Win;
#endif 

typedef struct{
    int x,y;
} Matched;

typedef struct{
    int len;
    int n;
    char * query;
    Matched * matched;
} MatchedList;

void searchMode();
void search_next();
void search_prev();
void search(Win * win, char * query);
#endif
