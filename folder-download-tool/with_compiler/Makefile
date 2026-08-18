# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -I src
SRC_DIR = src
LEXER_DIR = $(SRC_DIR)/lexer
BUILD_DIR = build

# 源文件
SOURCES = $(SRC_DIR)/main.c \
          $(LEXER_DIR)/lexer.c

# 对象文件
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# 目标可执行文件
TARGET = withc xx	
TARGET = withc
# 创建构建目录
$(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/lexer)
$(shell mkdir -p $(BUILD_DIR  $(BUILD _DIR)))
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lexer/%.o: $(LEXER_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

test: $(TARGET)
	./$(TARGET) examples/hello.with

.PHONY: clean test