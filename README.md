# calc demo

一个用于演示 C 项目工程化的计算器示例（加法/减法/乘法/除法/幂/阶乘/开方）。

## 目录结构
- `inc/calc.h` 接口声明
- `src/calc.c` 实现
- `src/main.c` 演示程序
- `Makefile` / `CMakeLists.txt` 两种构建方式

## 构建与运行

### 方式一：Makefile（GNU make + gcc）

```powershell
make            # 编译，产物 build/calc_demo.exe
make run        # 编译并运行
make clean      # 删除 build/ 目录及其中的目标文件
```

### 方式二：CMake

CMake 采用「源码外构建」，构建目录建议用 `cmake-build-xxx` 命名——`.gitignore` 里的 `cmake-build-*/` 规则已经覆盖，不会误提交。

```powershell
# 1) 配置 + 编译（本机默认生成器为 Visual Studio 2015 / MSVC）
cmake -S . -B cmake-build-debug
cmake --build cmake-build-debug --config Release

# 2) 运行
.\cmake-build-debug\Release\calc_demo.exe

# 3) 清理：只删编译产物，保留配置缓存（下次构建免于重新配置）
cmake --build cmake-build-debug --target clean --config Release

# 4) 彻底清理：连 CMakeCache 等配置一起删，相当于重新来过
Remove-Item -Recurse -Force cmake-build-debug
```

**指定用 gcc（与 Makefile 同一套工具链）**：

```powershell
cmake -S . -B cmake-build-mingw -G "MinGW Makefiles"
cmake --build cmake-build-mingw                      # 单配置生成器不需要 --config
.\cmake-build-mingw\calc_demo.exe
cmake --build cmake-build-mingw --target clean
```

> Git Bash 下把 `Remove-Item -Recurse -Force xxx` 换成 `rm -rf xxx` 即可。

**产物位置**（两种生成器不同）：

| 生成器 | 可执行文件 | 编译类型 |
|---|---|---|
| Visual Studio（多配置） | `cmake-build-debug/Release/calc_demo.exe` | 用 `--config Release` / `--config Debug` 指定 |
| MinGW Makefiles（单配置） | `cmake-build-mingw/calc_demo.exe` | 配置时用 `-DCMAKE_BUILD_TYPE=Debug` 指定 |

两种方式编译出的可执行文件行为一致，均开启 `-Wall -Wextra -Wpedantic`（MSVC 下为 `/W4`）。

## 开发状态
- 分支：`main`（稳定）/ `develop`（集成）/ `feature/*`（功能）
- 最近完成：`calc_sqrt`、CMake 构建支持
- 下一步：CMakePresets.json、单元测试