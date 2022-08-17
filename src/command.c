#include "command.h"

#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "util.h"
#include <string.h>

extern Win *cwin;

void commandMode() {
    char *query = prompt(":%s", NULL);
    if (query == NULL) {
        mode_switch(NORMAL);
        return;
    }

    char filename[128];

    mess_send(filename);
    if (!strcmp(query, "q")) {
        quit();
    } else if (!strcmp(query, "w")) {
        file_save(cwin->buf);
    } else if (sscanf(query, "w %s", filename) == 1) {
        cwin->buf->file.name = filename;
        file_save(cwin->buf);
    }

    mode_switch(NORMAL);
}
