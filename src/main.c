#include <stdio.h>
#include "calc.h"

static void show(int a, int b)
{
    printf("a = %d, b = %d\n", a, b);
    printf("  a + b      = %d\n", calc_add(a, b));
    printf("  a - b      = %d\n", calc_sub(a, b));
    printf("  a * b      = %d\n", calc_mul(a, b));
    printf("  a / b      = %.4f\n", calc_div(a, b));
}

int main(void)
{
    printf("=== calc demo ===\n\n");
    show(12, 5);
    printf("\n");
    show(7, 0);
    printf("\n");
    printf("2 ^ 10     = %lld\n", calc_pow(2, 10));
    printf("10!        = %lld\n", calc_fact(10));
    printf("sqrt(2)    = %.4f\n", calc_sqrt(2.0));
    printf("sqrt(9)    = %.4f\n", calc_sqrt(9.0));
    printf("sqrt(-4)   = %.4f\n", calc_sqrt(-4.0));
    printf("\n");
    return 0;
}
