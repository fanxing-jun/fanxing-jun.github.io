#ifndef UTILS_H
#define UTILS_H

#include "vector.h"
#include "string_builder.h"

// 通用释放函数
void free_nothing(void* ptr);  // 用于不需要释放元素的vector
void free_direct(void* ptr);   // 直接free

// 字符串相关的释放函数
void free_string(void* ptr);

// 内存分配包装（便于后续添加内存调试）
void* xmalloc(size_t size);
void* xcalloc(size_t count, size_t size);
void* xrealloc(void* ptr, size_t size);
char* xstrdup(const char* str);

#endif