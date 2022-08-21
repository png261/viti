#include "buffer.h"

#include "memory.h"

#include <stdlib.h>

Buffer *buffer_create() 
{
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->line = 0;
    buf->col = 0;
    buf->file.lines = 1;
    buf->file.name = NULL;
    buf->rows = xmalloc(sizeof(*buf->rows));
    return buf;
}
