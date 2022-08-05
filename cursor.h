typedef struct{
    int x;
    int y;
} Cursor;

void cursor_refresh(Cursor * Cur);
void cursor_left(Cursor * Cur);
void cursor_right(Cursor * Cur);
void cursor_up(Cursor * Cur);
void cursor_down(Cursor * Cur);
