#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <errno.h>

#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define LEN(A)			        (sizeof(A) / sizeof(A)[0])
/* #define RANGE(X,MIN, MAX)       ((X) <= (MIN) ? (MIN) :(X) >= (MAX) ? (MAX) : (X)) */
#define LIMIT(x, MIN, MAX)		    (x) = (x) < (MIN) ? (MIN) : (x) > (MAX) ? (MAX) : (x)
#define CTRL(k) ((k)&0x1f)

void die(const char *errstr, ...);
void quit();

void * xrealloc(void *p, size_t len);
void * xmalloc(size_t len);
void * xcalloc(size_t n,size_t len);

#endif
