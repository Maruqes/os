#include "math.h"
double sqrt(double x)
{
    double res;
    asm("fsqrt"
        : "=t"(res)
        : "0"(x));
    return res;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a > b) ? b : a;
}

int abs(int a)
{
    return (a >= 0) ? a : -a;
}

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
