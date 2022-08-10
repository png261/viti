#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"
typedef struct buffer Buffer;

typedef struct {
    int x;
    int y;
} Cursor;

void cursor_refresh(Buffer * buf);
void cursor_left(Buffer * buf);
void cursor_right(Buffer * buf);
void cursor_up(Buffer * buf);
void cursor_down(Buffer * buf);

#endif

