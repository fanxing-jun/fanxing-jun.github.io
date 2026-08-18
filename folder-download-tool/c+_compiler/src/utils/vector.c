#include "vector.h"
#include <string.h>

#define VECTOR_INIT_CAPACITY 8
#define VECTOR_GROWTH_FACTOR 2

static bool vector_resize(Vector* vec, size_t new_capacity) {
    void** new_data = realloc(vec->data, sizeof(void*) * new_capacity);
    if (!new_data && new_capacity > 0) {
        return false;
    }
    vec->data = new_data;
    vec->capacity = new_capacity;
    return true;
}

Vector* vector_create(void (*free_fn)(void*)) {
    Vector* vec = malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->size = 0;
    vec->capacity = VECTOR_INIT_CAPACITY;
    vec->free_fn = free_fn;
    vec->data = malloc(sizeof(void*) * vec->capacity);
    
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    
    return vec;
}

Vector* vector_with_capacity(size_t capacity, void (*free_fn)(void*)) {
    Vector* vec = malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->size = 0;
    vec->capacity = capacity > 0 ? capacity : 1;
    vec->free_fn = free_fn;
    vec->data = malloc(sizeof(void*) * vec->capacity);
    
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    
    return vec;
}

void vector_free(Vector* vec) {
    if (!vec) return;
    
    if (vec->free_fn) {
        for (size_t i = 0; i < vec->size; i++) {
            vec->free_fn(vec->data[i]);
        }
    }
    
    free(vec->data);
    free(vec);
}

void* vector_get(const Vector* vec, size_t index) {
    if (!vec || index >= vec->size) {
        return NULL;
    }
    return vec->data[index];
}

void vector_set(Vector* vec, size_t index, void* element) {
    if (!vec || index >= vec->size) {
        return;
    }
    
    // 释放旧元素（如果设置了释放函数）
    if (vec->free_fn) {
        vec->free_fn(vec->data[index]);
    }
    
    vec->data[index] = element;
}

void vector_push(Vector* vec, void* element) {
    if (!vec) return;
    
    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity * VECTOR_GROWTH_FACTOR;
        if (!vector_resize(vec, new_capacity)) {
            return;
        }
    }
    
    vec->data[vec->size++] = element;
}

void* vector_pop(Vector* vec) {
    if (!vec || vec->size == 0) {
        return NULL;
    }
    
    return vec->data[--vec->size];
}

bool vector_reserve(Vector* vec, size_t additional) {
    if (!vec) return false;
    
    if (vec->size + additional > vec->capacity) {
        size_t new_capacity = vec->size + additional;
        return vector_resize(vec, new_capacity);
    }
    
    return true;
}

void vector_shrink_to_fit(Vector* vec) {
    if (!vec || vec->size == vec->capacity) return;
    
    vector_resize(vec, vec->size);
}

size_t vector_size(const Vector* vec) {
    return vec ? vec->size : 0;
}

size_t vector_capacity(const Vector* vec) {
    return vec ? vec->capacity : 0;
}

bool vector_is_empty(const Vector* vec) {
    return !vec || vec->size == 0;
}

void vector_clear(Vector* vec) {
    if (!vec) return;
    
    if (vec->free_fn) {
        for (size_t i = 0; i < vec->size; i++) {
            vec->free_fn(vec->data[i]);
        }
    }
    
    vec->size = 0;
}

void* vector_remove(Vector* vec, size_t index) {
    if (!vec || index >= vec->size) {
        return NULL;
    }
    
    void* element = vec->data[index];
    
    // 移动后续元素
    for (size_t i = index; i < vec->size - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }
    
    vec->size--;
    return element;
}

void vector_insert(Vector* vec, size_t index, void* element) {
    if (!vec || index > vec->size) {
        return;
    }
    
    if (!vector_reserve(vec, 1)) {
        return;
    }
    
    // 移动元素腾出空间
    for (size_t i = vec->size; i > index; i--) {
        vec->data[i] = vec->data[i - 1];
    }
    
    vec->data[index] = element;
    vec->size++;
}

// 迭代器实现
VectorIterator vector_iterator(Vector* vec) {
    VectorIterator it = {vec, 0};
    return it;
}

bool vector_iterator_has_next(VectorIterator* it) {
    return it && it->vec && it->index < it->vec->size;
}

void* vector_iterator_next(VectorIterator* it) {
    if (!vector_iterator_has_next(it)) {
        return NULL;
    }
    return it->vec->data[it->index++];
}