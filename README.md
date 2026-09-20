# calc demo

![CI](https://github.com/ggzzc/git_test/actions/workflows/ci.yml/badge.svg)

一个用于演示 C 项目工程化的计算器示例（加法/减法/乘法/除法/幂/阶乘/开方）。

## 目录结构
- `inc/calc.h` 接口声明
- `src/calc.c` 实现
- `src/main.c` 演示程序
- `tests/test_calc.c` 单元测试（零依赖，27 条断言）
- `Makefile` / `CMakeLists.txt` / `CMakePresets.json` 构建与预设
- `.clang-tidy` 静态检查规则（本地与 CI 共用）
- `.github/workflows/ci.yml` 持续集成（7 个作业）
- `.github/workflows/release.yml` 持续发布（打 `v*` 标签触发）

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

## 持续集成 / 持续发布

分支模型：`main` 是发布线，`develop` 是集成线，功能开发从 `develop` 开出 `feature/*`。
**`main` 只接收来自 `develop` 的合并**，合并后打 tag 发布。

**CI** —— `.github/workflows/ci.yml`，push / PR 触发，共 7 个作业：

| 作业 | 内容 |
|---|---|
| `build-and-test` | 4 平台编译 + 单元测试：linux-gcc / linux-clang / macos-clang / windows-msvc |
| `strict-warnings` | 告警即错误：GCC/Clang 的 `-Werror`、MSVC 的 `/WX`（`ENABLE_WERROR=ON`） |
| `cppcheck` | 源码级静态分析，`--error-exitcode=1`，有发现即失败 |
| `clang-tidy` | 语义级检查，规则见 `.clang-tidy`（本地跑同样命令可复现结论） |
| `sanitizers` | ASan + UBSan 运行时检查（gcc / clang），抓越界、泄漏、未定义行为 |
| `valgrind` | memcheck：内存泄漏、未初始化读取（Linux） |
| `coverage` | gcov + gcovr，XML/HTML 报告作为 artifact 上传 |

> 动态检查（sanitizer / valgrind）只跑 Linux：MinGW 没有 ASan，MSVC 不支持 UBSan。

**CD** —— `.github/workflows/release.yml`，推送 `v*` 标签触发：

```bash
git tag -a v0.2.3 -m "v0.2.3: ..."
git push --follow-tags
```

流程：矩阵编译 linux / windows → 各自上传 artifact → 单个 `publish` 作业合并全部产物、
生成覆盖所有平台的 `SHA256SUMS.txt`、创建 GitHub Release 并上传。

## 开发状态
- 最近完成：`calc_sqrt`、CMake 构建与 CMakePresets、单元测试、完整 CI/CD（CI 7 作业 + tag 自动发布）
- 下一步：为 `main` / `develop` 启用分支保护（Require PR + Require status checks，并禁止直推 `main`）
- 可选的后续：更多数学函数（含溢出检测）、把 CI 徽章状态写进本文档、给覆盖率设下限（`gcovr --fail-under-line`）