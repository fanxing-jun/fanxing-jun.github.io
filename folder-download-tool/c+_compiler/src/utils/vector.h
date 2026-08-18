#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    void (*free_fn)(void*);  // 可选的元素释放函数
} Vector;

// 创建和销毁
Vector* vector_create(void (*free_fn)(void*));
Vector* vector_with_capacity(size_t capacity, void (*free_fn)(void*));
void vector_free(Vector* vec);

// 基本操作
void* vector_get(const Vector* vec, size_t index);
void vector_set(Vector* vec, size_t index, void* element);
void vector_push(Vector* vec, void* element);
void* vector_pop(Vector* vec);

// 容量管理
bool vector_reserve(Vector* vec, size_t additional);
void vector_shrink_to_fit(Vector* vec);

// 实用功能
size_t vector_size(const Vector* vec);
size_t vector_capacity(const Vector* vec);
bool vector_is_empty(const Vector* vec);
void vector_clear(Vector* vec);
void* vector_remove(Vector* vec, size_t index);
void vector_insert(Vector* vec, size_t index, void* element);

// 迭代器
typedef struct {
    Vector* vec;
    size_t index;
} VectorIterator;

VectorIterator vector_iterator(Vector* vec);
bool vector_iterator_has_next(VectorIterator* it);
void* vector_iterator_next(VectorIterator* it);

#endif