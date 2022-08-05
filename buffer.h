#include <ncurses.h>

typedef struct {
    char *content;
    int size;
} row;

typedef struct{
    int x;
    int y;
    int xoff;
    int yoff;
} View;

typedef struct {
    row *rows;
    View view;
    int nlines;
    WINDOW *win;
} Buffer;

void buffer_render(Buffer * buf); 

