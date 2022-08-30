// util.c: some util defines and functions

#ifndef VITI_UTIL_H
#define VITI_UTIL_H

#include <errno.h>
#include <stddef.h>


#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define LEN(A)			        (sizeof(A) / sizeof(A)[0])
#define LIMIT(X, MIN, MAX)		((X) = (X) < (MIN) ? (MIN) : (X) > (MAX) ? (MAX) : (X))


void viti_die(const char *errstr, ...);
void viti_quit();

size_t trim(char *str); 

#endif // VITI_UTIL_H
