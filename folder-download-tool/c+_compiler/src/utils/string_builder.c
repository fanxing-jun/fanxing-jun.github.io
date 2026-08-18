#include "string_builder.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define SB_INIT_CAPACITY 16
#define SB_GROWTH_FACTOR 2

static bool sb_ensure_capacity(StringBuilder* sb, size_t additional) {
    if (sb->length + additional + 1 <= sb->capacity) {
        return true;
    }
    
    size_t new_capacity = sb->capacity;
    while (new_capacity < sb->length + additional + 1) {
        new_capacity *= SB_GROWTH_FACTOR;
    }
    
    char* new_buffer = realloc(sb->buffer, new_capacity);
    if (!new_buffer) {
        return false;
    }
    
    sb->buffer = new_buffer;
    sb->capacity = new_capacity;
    return true;
}

StringBuilder* sb_create(void) {
    return sb_create_with_capacity(SB_INIT_CAPACITY);
}

StringBuilder* sb_create_with_capacity(size_t capacity) {
    StringBuilder* sb = malloc(sizeof(StringBuilder));
    if (!sb) return NULL;
    
    sb->capacity = capacity > 0 ? capacity : 1;
    sb->length = 0;
    sb->buffer = malloc(sb->capacity);
    
    if (!sb->buffer) {
        free(sb);
        return NULL;
    }
    
    sb->buffer[0] = '\0';
    return sb;
}

void sb_free(StringBuilder* sb) {
    if (!sb) return;
    free(sb->buffer);
    free(sb);
}

bool sb_append(StringBuilder* sb, const char* str) {
    if (!sb || !str) return false;
    
    size_t len = strlen(str);
    if (!sb_ensure_capacity(sb, len)) {
        return false;
    }
    
    memcpy(sb->buffer + sb->length, str, len);
    sb->length += len;
    sb->buffer[sb->length] = '\0';
    return true;
}

bool sb_append_char(StringBuilder* sb, char c) {
    if (!sb) return false;
    
    if (!sb_ensure_capacity(sb, 1)) {
        return false;
    }
    
    sb->buffer[sb->length++] = c;
    sb->buffer[sb->length] = '\0';
    return true;
}

bool sb_append_int(StringBuilder* sb, int value) {
    char buffer[32];
    int len = snprintf(buffer, sizeof(buffer), "%d", value);
    return sb_append(sb, buffer);
}

bool sb_append_double(StringBuilder* sb, double value) {
    char buffer[64];
    int len = snprintf(buffer, sizeof(buffer), "%g", value);
    return sb_append(sb, buffer);
}

bool sb_append_format(StringBuilder* sb, const char* format, ...) {
    if (!sb || !format) return false;
    
    va_list args;
    va_start(args, format);
    
    // 获取格式化后的长度
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    
    if (needed < 0) {
        va_end(args);
        return false;
    }
    
    if (!sb_ensure_capacity(sb, needed)) {
        va_end(args);
        return false;
    }
    
    // 实际格式化
    int written = vsnprintf(sb->buffer + sb->length, needed + 1, format, args);
    va_end(args);
    
    if (written < 0) {
        return false;
    }
    
    sb->length += written;
    return true;
}

bool sb_reserve(StringBuilder* sb, size_t additional) {
    return sb_ensure_capacity(sb, additional);
}

void sb_clear(StringBuilder* sb) {
    if (!sb) return;
    sb->length = 0;
    sb->buffer[0] = '\0';
}

char* sb_to_string(const StringBuilder* sb) {
    if (!sb) return NULL;
    
    char* str = malloc(sb->length + 1);
    if (!str) return NULL;
    
    memcpy(str, sb->buffer, sb->length + 1);
    return str;
}

size_t sb_length(const StringBuilder* sb) {
    return sb ? sb->length : 0;
}

bool sb_is_empty(const StringBuilder* sb) {
    return !sb || sb->length == 0;
}

bool sb_insert(StringBuilder* sb, size_t pos, const char* str) {
    if (!sb || !str || pos > sb->length) return false;
    
    size_t len = strlen(str);
    if (!sb_ensure_capacity(sb, len)) {
        return false;
    }
    
    // 移动现有内容
    memmove(sb->buffer + pos + len, sb->buffer + pos, sb->length - pos + 1);
    
    // 插入新内容
    memcpy(sb->buffer + pos, str, len);
    sb->length += len;
    return true;
}

bool sb_remove(StringBuilder* sb, size_t pos, size_t len) {
    if (!sb || pos >= sb->length || len == 0) return false;
    
    size_t actual_len = len;
    if (pos + len > sb->length) {
        actual_len = sb->length - pos;
    }
    
    // 移动后续内容
    memmove(sb->buffer + pos, sb->buffer + pos + actual_len, 
            sb->length - pos - actual_len + 1);
    
    sb->length -= actual_len;
    return true;
}