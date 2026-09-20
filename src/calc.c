#include "calc.h"

int calc_add(int a, int b)
{
    return a + b;
}

int calc_sub(int a, int b)
{
    return a - b;
}

int calc_mul(int a, int b)
{
    return a * b;
}

double calc_div(int a, int b)
{
    if (b == 0) {
        return 0.0;
    }
    return (double)a / (double)b;
}

long long calc_pow(int base, int exp)
{
    long long result = 1;
    int i;
    if (exp < 0) {
        return 0;
    }
    for (i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

long long calc_fact(int n)
{
    long long result = 1;
    int i;
    if (n < 0) {
        return 0;
    }
    for (i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
