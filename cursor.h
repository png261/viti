#ifndef CURSOR_H
#define CURSOR_H

#include "buffer.h"
typedef struct buffer Buffer;

typedef struct {
    int x;
    int y;
} Cursor;

void cursor_refresh(Buffer * buf);

#endif

