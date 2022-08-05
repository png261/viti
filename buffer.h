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
    char * filename;
    row *rows;
    View view;
    int nlines;

    WINDOW *num;
    WINDOW *win;
    WINDOW *statusline;
} Buffer;

void buffer_render(Buffer * buf); 
void buffer_render_num(Buffer * buf); 
void buffer_render_rows(Buffer * buf); 
void buffer_render_statusline(Buffer * buf); 
Buffer buffer_create(int height, int width, int y, int x);

