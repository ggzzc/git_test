# ------------------------------------------------------------------
# calc demo —— CMake 构建脚本
# 与根目录的 Makefile 等价：
#   Makefile:  gcc -std=c11 -Wall -Wextra -Wpedantic -Iinc src/*.c -o build/calc_demo.exe
#   CMake:     cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug --config Release
#   测试:      ctest --test-dir cmake-build-debug -C Release --output-on-failure
# 产物位置：
#   多配置生成器（Visual Studio）：<构建目录>/Release/calc_demo.exe
#   单配置生成器（MinGW Makefiles / Ninja）：<构建目录>/calc_demo.exe
# 说明：构建目录用 cmake-build-xxx 命名，
#       已被 .gitignore 第 64 行的 "cmake-build-*/" 规则覆盖，不会误提交。
# ------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

project(calc_demo LANGUAGES C)

# --- 语言标准：对应 Makefile 的 -std=c11 ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# OFF = 关闭 GNU 扩展，等价于 -std=c11（而不是 -std=gnu11）
set(CMAKE_C_EXTENSIONS OFF)

# --- 统一的编译告警选项 ---
# 用 INTERFACE 库承载：谁 target_link_libraries 谁就继承这些选项，
# 不必在每个目标上重复写一遍（演示程序、单元测试都受益）。
add_library(calc_warnings INTERFACE)

# 按编译器分派，避免把 GNU 风格选项传给 MSVC 导致构建失败
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(calc_warnings INTERFACE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    # /W4：警告级别，大致对应 -Wall -Wextra
    target_compile_options(calc_warnings INTERFACE /W4)

    if(MSVC_VERSION GREATER_EQUAL 1910)
        # VS2017 及以上：源码与执行字符集都按 UTF-8 处理，
        # 否则中文注释在中文 Windows（代码页 936）下会报 warning C4819
        target_compile_options(calc_warnings INTERFACE /utf-8)
    else()
        # VS2015 RTM 等老版本不认识 /utf-8（会报 D9002），只能显式忽略 C4819
        target_compile_options(calc_warnings INTERFACE /wd4819)
    endif()
endif()

# --- 可选：把编译告警当作错误 ---
# CI 的 strict-warnings 作业开启它；本地也可随时打开做自检：
#   cmake -S . -B build-strict -DENABLE_WERROR=ON
#   或者： make cb P=mingw  之前先在预设里加该变量（见 CMakePresets.json）
option(ENABLE_WERROR "把编译告警当作错误处理（GCC/Clang: -Werror，MSVC: /WX）" OFF)
if(ENABLE_WERROR)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        target_compile_options(calc_warnings INTERFACE -Werror)
    elseif(MSVC)
        target_compile_options(calc_warnings INTERFACE /WX)
    endif()
endif()

# --- 核心运算做成静态库：演示程序和单元测试共用同一份实现 ---
add_library(calc STATIC src/calc.c)
# PUBLIC：链接 calc 的目标自动获得 -Iinc（对应 Makefile 的 -Iinc）
target_include_directories(calc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(calc PRIVATE calc_warnings)

# --- 可执行目标：源码清单只剩 main.c，运算实现在 calc 库里 ---
add_executable(calc_demo src/main.c)
target_link_libraries(calc_demo PRIVATE calc calc_warnings)

# --- 可选：覆盖率（gcov），供 CI 的 coverage 作业使用 ---
#   cmake -S . -B build-cov -DENABLE_COVERAGE=ON
option(ENABLE_COVERAGE "开启 gcov 覆盖率统计（仅 GCC/Clang）" OFF)
if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        # --coverage 既是编译选项也是链接选项，PUBLIC 会传递给使用者
        target_compile_options(calc PUBLIC --coverage)
        target_link_options(calc PUBLIC --coverage)
    else()
        message(WARNING "ENABLE_COVERAGE 仅在 GCC/Clang 下有效，本次已忽略")
    endif()
endif()

# --- 头文件依赖跟踪 ---
# Makefile 里靠 -MMD -MP 手工生成 .d 文件；CMake 内置该机制，无需额外配置。

# --- 可选：生成 compile_commands.json，供 clangd / clang-tidy 使用 ---
# （该文件已被 .gitignore 第 87 行忽略）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- 测试：启用 CTest，并加入 tests 子目录 ---
enable_testing()
add_subdirectory(tests)
