#include "utils.h"
#include <stdlib.h>
#include <string.h>

void free_nothing(void* ptr) {
    // 什么都不做，用于不需要释放元素的场景
    (void)ptr;
}

void free_direct(void* ptr) {
    free(ptr);
}

void free_string(void* ptr) {
    free(ptr);
}

void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr && size > 0) {
        // 这里可以添加错误处理或退出
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void* xcalloc(size_t count, size_t size) {
    void* ptr = calloc(count, size);
    if (!ptr && count > 0 && size > 0) {
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void* xrealloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        exit(EXIT_FAILURE);
    }
    return new_ptr;
}

char* xstrdup(const char* str) {
    if (!str) return NULL;
    char* new_str = malloc(strlen(str) + 1);
    if (!new_str) {
        exit(EXIT_FAILURE);
    }
    strcpy(new_str, str);
    return new_str;
}