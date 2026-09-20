# ------------------------------------------------------------------
# calc demo —— CMake 构建脚本
# 与根目录的 Makefile 等价：
#   Makefile:  gcc -std=c11 -Wall -Wextra -Wpedantic -Iinc src/*.c -o build/calc_demo.exe
#   CMake:     cmake -S . -B cmake-build-debug && cmake --build cmake-build-debug --config Release
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

# --- 可执行目标：源码清单与 Makefile 的 SOURCES 一致 ---
add_executable(calc_demo
    src/main.c
    src/calc.c
)

# --- 头文件搜索路径：对应 -Iinc ---
target_include_directories(calc_demo
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# --- 警告选项：对应 -Wall -Wextra -Wpedantic ---
# 按编译器分派，避免把 GNU 风格选项传给 MSVC 导致构建失败
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(calc_demo PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    # /W4：警告级别，大致对应 -Wall -Wextra
    target_compile_options(calc_demo PRIVATE /W4)

    if(MSVC_VERSION GREATER_EQUAL 1910)
        # VS2017 及以上：源码与执行字符集都按 UTF-8 处理，
        # 否则中文注释在中文 Windows（代码页 936）下会报 warning C4819
        target_compile_options(calc_demo PRIVATE /utf-8)
    else()
        # VS2015 RTM 等老版本不认识 /utf-8（会报 D9002），只能显式忽略 C4819
        target_compile_options(calc_demo PRIVATE /wd4819)
    endif()
endif()

# --- 头文件依赖跟踪 ---
# Makefile 里靠 -MMD -MP 手工生成 .d 文件；CMake 内置该机制，无需额外配置。

# --- 可选：生成 compile_commands.json，供 clangd / IDE 做代码索引 ---
# （该文件已被 .gitignore 第 87 行忽略）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
