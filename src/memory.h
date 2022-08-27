#ifndef VITI_MEMORY_H
#define VITI_MEMORY_H

#include <stddef.h>


void *xrealloc(void *p, const size_t len);
void *xcalloc(const size_t n, const size_t len);
void *xmalloc(const size_t len);

#endif // VITI_MEMORY_H
