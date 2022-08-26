#ifndef VITI_UTIL_H
#define VITI_UTIL_H

#include <stddef.h>
#include <errno.h>


#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define LEN(A)			        (sizeof(A) / sizeof(A)[0])
#define LIMIT(X, MIN, MAX)		((X) = (X) < (MIN) ? (MIN) : (X) > (MAX) ? (MAX) : (X))


void die(const char *errstr, ...);
void quit();

int countLines(const char *filename);
size_t trim(char *str); 
#endif
