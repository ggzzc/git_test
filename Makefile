CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinc
LDFLAGS :=

BUILD_DIR := build
TARGET := $(BUILD_DIR)/calc_demo.exe
SOURCES := src/main.c src/calc.c
OBJECTS := $(SOURCES:src/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

TEST_TARGET := $(BUILD_DIR)/test_calc.exe

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

# 编译并运行单元测试：测试失败时程序返回非 0，make 随之报错
test: | $(BUILD_DIR)
	$(CC) $(CFLAGS) tests/test_calc.c src/calc.c -o $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET)

# ==================================================================
# CMake 快捷入口（内部调用 CMakePresets.json 里的预设）
#   make cb          配置 + 编译
#   make ct          配置 + 编译 + 跑单元测试
#   make cr          配置 + 编译 + 运行 demo
#   make cc          删除所有 cmake-build-* 目录
#   make cb P=msvc   换用别的预设（mingw / msvc / coverage）
# 说明：上面的 all / run / test / clean 是直接用 gcc 编译的老通道；
#       这一节走 CMake 通道，两者互不影响。
# ==================================================================
P ?= mingw
CB_DIR := cmake-build-$(P)

.PHONY: cb ct cr cc

cb:
	cmake --preset $(P)
	cmake --build --preset $(P)

ct: cb
	ctest --preset $(P)

cr: cb
	@if [ -f $(CB_DIR)/calc_demo.exe ]; then ./$(CB_DIR)/calc_demo.exe; \
	 else ./$(CB_DIR)/Release/calc_demo.exe; fi

cc:
	$(RM) -r cmake-build-*

-include $(DEPS)