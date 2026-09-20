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

double calc_sqrt(double x)
{
    double r;
    double next;
    int i;

    if (x <= 0.0) {
        return 0.0;   /* 负数无实数平方根，与 calc_div 的约定一致 */
    }

    /* 牛顿迭代：r(n+1) = (r(n) + x / r(n)) / 2 */
    r = x;
    for (i = 0; i < 40; i++) {
        next = 0.5 * (r + x / r);
        if (next == r) {
            break;    /* 已达到 double 精度上限 */
        }
        r = next;
    }
    return r;
}
