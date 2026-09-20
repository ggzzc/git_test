# ------------------------------------------------------------------
# 单元测试目标
#   - 链接根目录定义的 calc 静态库（核心运算）
#   - 一并链接 calc_warnings，继承统一的告警选项
#   - 交给 CTest 驱动：ctest --test-dir <构建目录> -C Release --output-on-failure
# ------------------------------------------------------------------
add_executable(test_calc test_calc.c)

target_link_libraries(test_calc
    PRIVATE
        calc
        calc_warnings
)

add_test(NAME calc_unit COMMAND test_calc)

# 双保险：即使进程返回 0，只要输出里出现 FAIL 也算失败
set_tests_properties(calc_unit PROPERTIES
    FAIL_REGULAR_EXPRESSION "FAIL"
)
