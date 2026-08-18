#include "symbol_table.h"
#include "../utils/string_builder.h"
#include <stdio.h>

// 符号操作专用函数

// 创建变量符号
Symbol* symbol_create_variable(const char* name, Node* node, Type* type, bool is_mutable, bool is_constant) {
    Symbol* symbol = symbol_create(SYMBOL_VARIABLE, name, node, type);
    if (symbol) {
        symbol->variable.is_mutable = is_mutable;
        symbol->variable.is_constant = is_constant;
    }
    return symbol;
}

// 创建函数符号
Symbol* symbol_create_function(const char* name, Node* node, bool is_async, 
                              Vector* param_types, Type* return_type) {
    Symbol* symbol = symbol_create(SYMBOL_FUNCTION, name, node, NULL);
    if (symbol) {
        symbol->function.is_async = is_async;
        if (param_types) {
            // 转移所有权
            vector_free(symbol->function.param_types);
            symbol->function.param_types = param_types;
        }
        symbol->function.return_type = return_type;
        
        // 创建函数的类型
        Type* func_type = type_create_function(param_types, return_type);
        symbol->symbol_type = func_type;
    }
    return symbol;
}

// 创建结构体符号
Symbol* symbol_create_struct(const char* name, Node* node, Vector* generic_params) {
    Symbol* symbol = symbol_create(SYMBOL_STRUCT, name, node, NULL);
    if (symbol && generic_params) {
        // 转移所有权
        vector_free(symbol->structure.generic_params);
        symbol->structure.generic_params = generic_params;
    }
    return symbol;
}

// 创建类型参数符号
Symbol* symbol_create_type_param(const char* name, Node* node, Type* constraint) {
    Symbol* symbol = symbol_create(SYMBOL_TYPE_PARAM, name, node, NULL);
    if (symbol) {
        symbol->type_param.constraint = constraint;
    }
    return symbol;
}

// 设置符号可见性
void symbol_set_visibility(Symbol* symbol, SymbolVisibility visibility) {
    if (symbol) {
        symbol->visibility = visibility;
    }
}

// 检查符号是否可访问
bool symbol_is_accessible(const Symbol* symbol, const Scope* current_scope) {
    if (!symbol || !current_scope) return false;
    
    switch (symbol->visibility) {
        case VISIBILITY_PUBLIC:
            return true;
            
        case VISIBILITY_MODULE:
            // 简化处理：假设在同一模块内
            return true;
            
        case VISIBILITY_PRIVATE:
            // 私有符号只能在定义它的作用域或其子作用域中访问
            return scope_is_descendant(current_scope, symbol->scope);
            
        default:
            return false;
    }
}

// 检查作用域是否是另一个作用域的后代
bool scope_is_descendant(const Scope* descendant, const Scope* ancestor) {
    const Scope* current = descendant;
    while (current) {
        if (current == ancestor) {
            return true;
        }
        current = current->parent;
    }
    return false;
}

// 获取符号的完整名称（包含作用域信息）
char* symbol_get_full_name(const Symbol* symbol) {
    if (!symbol || !symbol->name) return NULL;
    
    StringBuilder* sb = sb_create();
    
    // 从符号的作用域向上构建完整名称
    Vector* scope_names = vector_create(free_direct);
    Scope* current = symbol->scope;
    
    while (current && !scope_is_global(current)) {
        const char* scope_name = NULL;
        
        switch (current->type) {
            case SCOPE_FUNCTION:
                if (current->function_scope.function_symbol) {
                    scope_name = current->function_scope.function_symbol->name;
                }
                break;
                
            case SCOPE_STRUCT:
                if (current->struct_scope.struct_symbol) {
                    scope_name = current->struct_scope.struct_symbol->name;
                }
                break;
                
            case SCOPE_MODULE:
                if (current->module_scope.module_name) {
                    scope_name = current->module_scope.module_name;
                }
                break;
                
            default:
                break;
        }
        
        if (scope_name) {
            vector_push(scope_names, xstrdup(scope_name));
        }
        
        current = current->parent;
    }
    
    // 反向构建完整名称
    for (int i = vector_size(scope_names) - 1; i >= 0; i--) {
        char* name = vector_get(scope_names, i);
        sb_append(sb, name);
        sb_append(sb, "::");
    }
    
    sb_append(sb, symbol->name);
    
    char* full_name = sb_to_string(sb);
    
    vector_free(scope_names);
    sb_free(sb);
    
    return full_name;
}