#include "buffer.h"

#include "memory.h"

#include <stdlib.h>

Buffer * curbuf;

Buffer *buffer_create() 
{
    Buffer *buf = xmalloc(sizeof(*buf));
    buf->curline = 0;
    buf->curcol = 0;
    buf->nlines = 1;
    buf->file.name = NULL;
    buf->lines = xmalloc(sizeof(*buf->lines));
    return buf;
}
