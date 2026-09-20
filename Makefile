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

-include $(DEPS)