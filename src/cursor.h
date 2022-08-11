#ifndef CURSOR_H
#define CURSOR_H

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
#endif 

typedef struct {
    int x;
    int y;
} Cursor;

void cursor_refresh(Buffer * buf);
void cursor_left(Buffer * buf);
void cursor_right(Buffer * buf);
void cursor_up(Buffer * buf);
void cursor_down(Buffer * buf);

void go_to_line(Buffer * buf, int line);
#endif

