#ifndef FILE_IO_H
#define FILE_IO_H
#include "buffer.h"

#ifndef TYPEDEF_BUFFER
#define TYPEDEF_BUFFER
typedef struct buffer Buffer;
#endif 

void file_open(char *filename,Buffer *buf);
void file_save(Buffer *buf);

#endif
