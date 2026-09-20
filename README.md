# calc demo

一个用于演示 C 项目工程化的计算器示例（加法/减法/乘法/除法/幂/阶乘/开方）。

## 目录结构
- `inc/calc.h` 接口声明
- `src/calc.c` 实现
- `src/main.c` 演示程序
- `tests/test_calc.c` 单元测试（零依赖，27 条断言）
- `Makefile` / `CMakeLists.txt` / `CMakePresets.json` 构建与预设

## 快速上手（推荐）

```powershell
make ct          # 配置 + 编译 + 跑单元测试（gcc 预设）
make cr          # 配置 + 编译 + 运行 demo
make cc          # 清理：删除所有 cmake-build-* 目录
```

等价的一条 CMake 命令（配置 + 编译 + 测试，一次跑完）：

```powershell
cmake --workflow --preset check
```

## 构建与运行

三种方式任选，编译出的程序行为完全一致（都开启 `-Wall -Wextra -Wpedantic`，MSVC 下对应 `/W4`）。

### 方式一：make 快捷目标（最短）

| 命令 | 作用 | 说明 |
|---|---|---|
| `make cb` | 配置 + 编译 | 走 CMake 预设，默认 `mingw`（gcc） |
| `make ct` | 配置 + 编译 + 单元测试 | |
| `make cr` | 配置 + 编译 + 运行 demo | |
| `make cc` | 删除所有 `cmake-build-*` 目录 | 彻底清理 |
| `make cb P=msvc` | 换用其它预设 | 可选 `mingw` / `msvc` / `coverage` |

> `make` / `make run` / `make test` / `make clean` 是**不经过 CMake**、直接用 gcc 编译的通道，见方式三。

### 方式二：CMake Presets（需要分步控制时）

预设定义在 `CMakePresets.json`，构建目录固定为 `cmake-build-<预设名>`（已被 `.gitignore` 的 `cmake-build-*/` 覆盖，不会误提交）。

| 预设 | 生成器 / 编译器 | 构建类型 | 构建目录 |
|---|---|---|---|
| `mingw` | MinGW Makefiles / **gcc** | Release | `cmake-build-mingw/` |
| `msvc` | 默认生成器 / **Visual Studio (MSVC)** | 由 `configuration` 指定 | `cmake-build-msvc/` |
| `coverage` | MinGW Makefiles / gcc + gcov | Debug | `cmake-build-coverage/` |

```powershell
# 分步执行：把 mingw 换成 msvc / coverage 即可切换工具链
cmake --preset mingw            # 1) 配置
cmake --build --preset mingw    # 2) 编译
ctest --preset mingw            # 3) 测试

# 一条命令跑完三步
cmake --workflow --preset check

# 列出全部预设
cmake --list-presets=all
```

**清理（两个层次，对应 CMake 的产物与缓存）**：

```powershell
# 只删编译产物，保留 CMakeCache —— 下次构建无需重新配置
cmake --build --preset mingw --target clean

# 彻底清理：删除整个构建目录（含配置缓存）
Remove-Item -Recurse -Force cmake-build-mingw
# 一次清掉所有构建目录（等价写法）
make cc
```

**MSVC 通道**（多配置生成器，构建类型用参数指定）：

```powershell
cmake --preset msvc                          # 配置（本机默认生成器为 Visual Studio 2015）
cmake --build --preset msvc                  # 编译 Release（configuration 已在预设中固定）
ctest --preset msvc                          # 测试

# 想临时换成 Debug：
cmake --build cmake-build-msvc --config Debug
```

**产物位置**（多配置与单配置生成器不同）：

| 预设 | 可执行文件 | 单元测试程序 |
|---|---|---|
| `mingw` | `cmake-build-mingw/calc_demo.exe` | `cmake-build-mingw/tests/test_calc.exe` |
| `msvc` | `cmake-build-msvc/Release/calc_demo.exe` | `cmake-build-msvc/tests/Release/test_calc.exe` |

**覆盖率**（仅 GCC / Clang）：

```powershell
cmake --preset coverage
cmake --build --preset coverage
ctest --preset coverage
gcovr -r . --exclude 'tests/.*' --txt-summary      # 需先安装 gcovr
```

### 方式三：直接 Makefile 编译（gcc，不经过 CMake）

```powershell
make            # 用 gcc 直接编译，产物 build/calc_demo.exe
make run        # 编译并运行
make test       # 编译并运行单元测试，产物 build/test_calc.exe
make clean      # 删除 build/ 目录
```

### 附：不使用预设的原始命令

预设只是把下面这些参数固化下来。预设文件用了 workflow presets，需要 CMake 3.25+；版本较低时可以直接用下面的原始命令：

```powershell
# gcc（MinGW Makefiles，单配置）
cmake -S . -B cmake-build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-mingw
.\cmake-build-mingw\calc_demo.exe
ctest --test-dir cmake-build-mingw --output-on-failure

# MSVC（本机默认生成器，多配置）
cmake -S . -B cmake-build-msvc
cmake --build cmake-build-msvc --config Release
.\cmake-build-msvc\Release\calc_demo.exe
ctest --test-dir cmake-build-msvc -C Release --output-on-failure
```

> Git Bash 下把 `Remove-Item -Recurse -Force xxx` 换成 `rm -rf xxx` 即可。

## 单元测试

`tests/test_calc.c` 零依赖实现（不用 Unity / CMocka），覆盖 7 个函数的正常值与边界——除零、负指数、负数阶乘、负数开方、`0!`、`0 × n` 等，共 27 条断言；失败时程序返回非 0 并打印 `FAIL 文件:行号 表达式`。

```powershell
make ct                    # 配置 + 编译 + 测试（最短）
ctest --preset mingw       # 只跑测试
ctest --preset mingw -V    # 显示详细输出
make test                  # gcc 直编通道
```

失败时的输出形如：

```text
FAIL tests/test_calc.c:68  ABS_DIFF((calc_sqrt(-4.0)), (0.0)) < (1e-9)
27 checks, 1 failed
```

## 开发状态
- 分支：`main`（稳定）/ `develop`（集成）/ `feature/*`（功能）
- 工具链：gcc —— MinGW Makefiles（`mingw` 预设）；MSVC —— 默认生成器（`msvc` 预设）
- 构建：CMakePresets + make 快捷目标（`cb` / `ct` / `cr` / `cc`）；另有不经 CMake 的 gcc 直编通道
- 测试：`tests/test_calc.c`，由 CTest 驱动（`ctest --preset mingw`）
- 最近完成：`calc_sqrt`、CMake 构建、CMakePresets 与 make 快捷目标、单元测试
- 下一步：GitHub Actions CI（cppcheck / clang-tidy / ASan / Valgrind / 覆盖率）、Release 打包