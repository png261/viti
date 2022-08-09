#ifndef EDIT_H
#define EDIT_H
#include "buffer.h"

typedef struct buffer Buffer;

void append_char(Buffer *buf, int line, int col, char c);
void append_str(Buffer *buf, int line, char *str);
void del_char(Buffer *buf, int line, int col);

void add_line(Buffer *buf, int line);
void del_line(Buffer *buf, int line);

#endif
