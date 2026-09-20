CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Iinc
LDFLAGS :=

BUILD_DIR := build
TARGET := $(BUILD_DIR)/calc_demo.exe
SOURCES := src/main.c src/calc.c
OBJECTS := $(SOURCES:src/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET)

-include $(DEPS)