#include "buffer.h"

#include <stdlib.h>

Buffer *cbuf;

Row *current_row(Win *win) { return &win->buf->rows[current_line(win)]; }

Buffer *buffer_create() {
    Buffer *buf = malloc(sizeof(Buffer));
    buf->line = 0;
    buf->col = 0;
    buf->file.lines = 1;
    buf->file.name = NULL;
    buf->rows = malloc(sizeof(Row));
    buf->rows[0].content = malloc(sizeof(char));
    return buf;
}
