#ifndef FILE_IO_H
#define FILE_IO_H
#include "buffer.h"

typedef struct buffer Buffer;

void file_open(char *filename,Buffer *buf);
void file_save(Buffer *buf);

#endif
