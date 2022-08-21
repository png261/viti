#include "command.h"

#include "fileio.h"
#include "message.h"
#include "mode.h"
#include "search.h"
#include "util.h"

#include <string.h>

extern Win *curwin;
extern int is_highlight;

void command_mode() 
{
    const char *query = prompt(":%s", NULL);
    if (query == NULL) {
        mode_switch(MODE_NORMAL);
        return;
    }

    char filename[128];

    if (strcmp(query, "q") == 0) {
        quit();
    } else if (strcmp(query, "w") == 0) {
        file_save(curwin->buf->file.name, curwin->buf);
    } else if (strcmp(query, "noh") == 0) {
        is_highlight = false;
        win_render_rows(curwin);
    } else if (sscanf(query, "w %s", filename) == 1) {
        curwin->buf->file.name = filename;
        file_save(filename, curwin->buf);
        win_render_statusline(curwin);
    }

    mode_switch(MODE_NORMAL);
}
