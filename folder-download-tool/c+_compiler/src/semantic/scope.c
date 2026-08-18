#include "scope.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>

// 创建基础作用域
Scope* scope_create(ScopeType type, Scope* parent) {
    Scope* scope = malloc(sizeof(Scope));
    if (!scope) return NULL;
    
    scope->type = type;
    scope->parent = parent;
    scope->symbols = hash_map_create(free_direct, (void (*)(void*))symbol_free);
    scope->children = vector_create(free_direct);
    
    // 初始化特定信息
    switch (type) {
        case SCOPE_FUNCTION:
            scope->function_scope.function_symbol = NULL;
            scope->function_scope.has_return = false;
            break;
            
        case SCOPE_LOOP:
            scope->loop_scope.can_break = true;
            scope->loop_scope.can_continue = true;
            break;
            
        case SCOPE_STRUCT:
            scope->struct_scope.struct_symbol = NULL;
            break;
            
        case SCOPE_MODULE:
            scope->module_scope.module_name = NULL;
            break;
            
        default:
            break;
    }
    
    // 如果存在父作用域，将当前作用域添加到父作用域的子作用域列表中
    if (parent) {
        vector_push(parent->children, scope);
    }
    
    return scope;
}

// 创建函数作用域
Scope* scope_create_with_function(Scope* parent, Symbol* function_symbol) {
    Scope* scope = scope_create(SCOPE_FUNCTION, parent);
    if (scope && function_symbol) {
        scope->function_scope.function_symbol = function_symbol;
    }
    return scope;
}

// 创建结构体作用域
Scope* scope_create_with_struct(Scope* parent, Symbol* struct_symbol) {
    Scope* scope = scope_create(SCOPE_STRUCT, parent);
    if (scope && struct_symbol) {
        scope->struct_scope.struct_symbol = struct_symbol;
    }
    return scope;
}

// 创建循环作用域
Scope* scope_create_with_loop(Scope* parent, bool can_break, bool can_continue) {
    Scope* scope = scope_create(SCOPE_LOOP, parent);
    if (scope) {
        scope->loop_scope.can_break = can_break;
        scope->loop_scope.can_continue = can_continue;
    }
    return scope;
}

// 创建模块作用域
Scope* scope_create_with_module(Scope* parent, const char* module_name) {
    Scope* scope = scope_create(SCOPE_MODULE, parent);
    if (scope && module_name) {
        scope->module_scope.module_name = xstrdup(module_name);
    }
    return scope;
}

// 释放作用域
void scope_free(Scope* scope) {
    if (!scope) return;
    
    // 释放模块名称
    if (scope->type == SCOPE_MODULE && scope->module_scope.module_name) {
        free(scope->module_scope.module_name);
    }
    
    // 释放符号表和子作用域列表
    hash_map_free(scope->symbols);
    vector_free(scope->children);
    
    free(scope);
}

// 作用域查询函数
bool scope_is_global(const Scope* scope) {
    return scope && scope->type == SCOPE_GLOBAL;
}

bool scope_is_function(const Scope* scope) {
    return scope && scope->type == SCOPE_FUNCTION;
}

bool scope_is_loop(const Scope* scope) {
    return scope && scope->type == SCOPE_LOOP;
}

bool scope_is_struct(const Scope* scope) {
    return scope && scope->type == SCOPE_STRUCT;
}

// 在当前作用域查找符号
Symbol* scope_lookup_local(const Scope* scope, const char* name) {
    if (!scope || !name || !scope->symbols) {
        return NULL;
    }
    
    return hash_map_get(scope->symbols, name);
}

// 在作用域链中查找符号
Symbol* scope_lookup(const Scope* scope, const char* name) {
    const Scope* current = scope;
    while (current) {
        Symbol* symbol = scope_lookup_local(current, name);
        if (symbol) {
            return symbol;
        }
        current = current->parent;
    }
    return NULL;
}

// 在当前作用域定义符号
bool scope_define(Scope* scope, Symbol* symbol) {
    if (!scope || !symbol || !symbol->name || !scope->symbols) {
        return false;
    }
    
    // 检查是否已存在同名符号
    if (hash_map_contains(scope->symbols, symbol->name)) {
        return false;
    }
    
    hash_map_put(scope->symbols, symbol->name, symbol);
    return true;
}

// 遍历作用域
void scope_traverse(Scope* scope, ScopeVisitor visitor, void* context) {
    if (!scope || !visitor) return;
    
    visitor(scope, context);
    
    for (size_t i = 0; i < vector_size(scope->children); i++) {
        Scope* child = vector_get(scope->children, i);
        scope_traverse(child, visitor, context);
    }
}