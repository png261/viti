#ifndef VITI_FILEIO_H
#define VITI_FILEIO_H

#include "buffer.h"

#ifndef INCLUDE_TYPEDEF_BUFFER
#define INCLUDE_TYPEDEF_BUFFER
    typedef struct buffer Buffer;
#endif 

void file_open(char *filename, Buffer *buf);
void file_save(char *filename, Buffer *buf);

#endif // VITI_FILEIO_H
