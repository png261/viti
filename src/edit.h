#ifndef EDIT_H
#define EDIT_H
#include "buffer.h"
#include "window.h"

void append_char(int line, int col, char c);
void append_str(int line, char *str);
void del_char(int line, int col);

void join_line(int line);
void add_line(int line, char * str);
void del_line(int line);
void break_line(int line, int col);

char * del_end(int line, int col);
char * del_str(int line, int start, int end); 
#endif
