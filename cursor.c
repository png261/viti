#include "buffer.h"
#include "cursor.h"

void cursor_refresh(Buffer *buf) {
    wmove(buf->win, buf->cur.y, buf->cur.x);
    wrefresh(buf->win);
}
