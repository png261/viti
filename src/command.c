#include "buffer.h"
#include "command.h"
#include "file_io.h"
#include "mess.h"
#include "mode.h"
#include "util.h"
#include <string.h>

extern Buffer *cbuf;

void commandMode() {
    char *query = prompt(":%s");
    char filename[128];
    int toLine = 0;

    mess_send(filename);
    if (strcmp(query, "q") == 0) {
        quit();
    } else if (strcmp(query, "w") == 0) {
        file_save(cbuf);
    } else if (sscanf(query, "w %s", filename) != 0) {
        cbuf->file.name = filename;
        file_save(cbuf);
    } else if (sscanf(query, "%d", &toLine) != 0) {
        go_to_line(cbuf, toLine);
        mess_send(":%d", toLine);
    }

    mode_switch(NORMAL);
}
