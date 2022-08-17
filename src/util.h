#define MAX(A, B)               ((A) > (B) ? (A) : (B))
#define MIN(A, B)               ((A) < (B) ? (A) : (B))
#define RANGE(X,MIN, MAX)       ((X) <= (MIN) ? (MIN) :(X) >= (MAX) ? (MAX) : (X))
#define CTRL(k) ((k)&0x1f)

void die(const char *message );
void quit();



