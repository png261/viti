#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define RANGE(X,MIN, MAX)       ((X) < (MIN) ? (MIN) :(X) > (MAX) ? (MAX) : (X))

void die(const char *fmt, ...);
void quit();



