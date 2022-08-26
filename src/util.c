#include "util.h"

#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


void die(const char *errstr, ...) 
{
    clear();
    endwin();

    va_list ap;
    va_start(ap, errstr);
    vfprintf(stderr, errstr, ap);
    va_end(ap);
    exit(1);
}


void quit() 
{
    clear();
    endwin();
    exit(0);
}

int countLines(const char *filename) 
{
    FILE *fp = fopen(filename, "r");
    char *line = NULL;
    size_t linecap = 0;
    int count = 0;
    while ((getline(&line, &linecap, fp)) != -1) {
        count++;
    }

    free(line);
    fclose(fp);
    return count;
}


size_t trim(char *str) 
{
    char *c = str + strlen(str) - 1;
    while (*c == '\n' || *c == ' ') {
        *c = '\0';
    }
    return strlen(str);
}

