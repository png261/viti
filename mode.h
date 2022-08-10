#ifndef MODE_H
#define MODE_H

enum MODE {
    NORMAL,
    INSERT,
    COMMAND,
    SEARCH,
    PROMPT,
};

void mode_switch(int mode);

#endif
