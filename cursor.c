#include "cursor.h"
#include <ncurses.h>

void cursor_refresh(Cursor *Cur) {
    move(Cur->y, Cur->x);
    refresh();
}

void cursor_left(Cursor *Cur) {
    Cur->x--;
    cursor_refresh(Cur);
}
void cursor_right(Cursor *Cur) {
    Cur->x++;
    cursor_refresh(Cur);
}
void cursor_up(Cursor *Cur) {
    Cur->y--;
    cursor_refresh(Cur);
}
void cursor_down(Cursor *Cur) {
    Cur->y++;
    cursor_refresh(Cur);
}
