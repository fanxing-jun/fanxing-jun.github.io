#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    char* buffer;
    size_t length;
    size_t capacity;
} StringBuilder;

// 创建和销毁
StringBuilder* sb_create(void);
StringBuilder* sb_create_with_capacity(size_t capacity);
void sb_free(StringBuilder* sb);

// 追加操作
bool sb_append(StringBuilder* sb, const char* str);
bool sb_append_char(StringBuilder* sb, char c);
bool sb_append_int(StringBuilder* sb, int value);
bool sb_append_double(StringBuilder* sb, double value);

// 格式化追加
bool sb_append_format(StringBuilder* sb, const char* format, ...);

// 实用功能
bool sb_reserve(StringBuilder* sb, size_t additional);
void sb_clear(StringBuilder* sb);
char* sb_to_string(const StringBuilder* sb);
size_t sb_length(const StringBuilder* sb);
bool sb_is_empty(const StringBuilder* sb);

// 字符串操作
bool sb_insert(StringBuilder* sb, size_t pos, const char* str);
bool sb_remove(StringBuilder* sb, size_t pos, size_t len);

#endif