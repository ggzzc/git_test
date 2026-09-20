/*
 * calc 单元测试
 * ------------------------------------------------------------------
 * 设计要点：
 *   1) 零依赖：不用 Unity / CMocka 之类的框架，标准 C 就能编译
 *   2) 失败返回非 0，可直接被 CTest、make test、CI 调用
 *   3) 失败时打印文件名:行号 和 表达式原文，便于定位
 *   4) 自己算差值，不引入 math.h -> Linux 上不必再链接 -lm
 * ------------------------------------------------------------------
 */
#include <stdio.h>
#include "calc.h"

static int g_checks = 0;
static int g_failed = 0;

/*
 * 断言逻辑放在函数里，条件作为函数参数求值，
 * 而不是让宏展开成 if (常量)。
 * 否则优化构建下 MSVC 会报 warning C4127「条件表达式是常量」——
 * 例如 CHECK_NEAR(calc_sqrt(0.0), 0.0, 1e-9) 会被折叠成恒真表达式。
 */
static void check_impl(int ok, const char *file, int line, const char *expr)
{
    g_checks++;
    if (!ok) {
        g_failed++;
        printf("FAIL %s:%d  %s\n", file, line, expr);
    }
}

#define CHECK(expr) check_impl((expr) != 0, __FILE__, __LINE__, #expr)

/* 取两数之差（参数会求值两次，测试里都是简单表达式，可接受） */
#define ABS_DIFF(a, b) (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))
#define CHECK_NEAR(a, b, eps) CHECK(ABS_DIFF((a), (b)) < (eps))

int main(void)
{
    /* ---------- 四则运算 ---------- */
    CHECK(calc_add(12, 5) == 17);
    CHECK(calc_add(-3, 3) == 0);
    CHECK(calc_sub(12, 5) == 7);
    CHECK(calc_sub(5, 12) == -7);
    CHECK(calc_mul(12, 5) == 60);
    CHECK(calc_mul(-2, 3) == -6);
    CHECK(calc_mul(0, 99) == 0);

    /* ---------- 除法：含除零约定（返回 0.0） ---------- */
    CHECK_NEAR(calc_div(12, 5), 2.4, 1e-9);
    CHECK_NEAR(calc_div(-7, 2), -3.5, 1e-9);
    CHECK_NEAR(calc_div(1, 0), 0.0, 1e-9);
    CHECK_NEAR(calc_div(0, 5), 0.0, 1e-9);

    /* ---------- 幂：含负指数约定（返回 0） ---------- */
    CHECK(calc_pow(2, 10) == 1024);
    CHECK(calc_pow(2, 0) == 1);
    CHECK(calc_pow(-2, 3) == -8);
    CHECK(calc_pow(0, 5) == 0);
    CHECK(calc_pow(5, -1) == 0);

    /* ---------- 阶乘：含负数约定（返回 0） ----------
     * 已知限制：n 较大时 long long 会溢出，这里不测溢出行为 */
    CHECK(calc_fact(0) == 1);
    CHECK(calc_fact(1) == 1);
    CHECK(calc_fact(5) == 120);
    CHECK(calc_fact(10) == 3628800);
    CHECK(calc_fact(-1) == 0);

    /* ---------- 开方：含负数约定（返回 0.0） ---------- */
    CHECK_NEAR(calc_sqrt(9.0), 3.0, 1e-9);
    CHECK_NEAR(calc_sqrt(0.25), 0.5, 1e-12);
    CHECK_NEAR(calc_sqrt(1e6), 1000.0, 1e-9);
    CHECK_NEAR(calc_sqrt(2.0), 1.4142135623730951, 1e-12);
    CHECK_NEAR(calc_sqrt(0.0), 0.0, 1e-9);
    CHECK_NEAR(calc_sqrt(-4.0), 0.0, 1e-9);

    printf("%d checks, %d failed\n", g_checks, g_failed);
    return (g_failed == 0) ? 0 : 1;
}
