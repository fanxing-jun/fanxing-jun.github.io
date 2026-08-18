#include "symbol_table.h"
#include "scope.h"
#include "../utils/utils.h"
#include "../utils/string_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建符号表
SymbolTable* symbol_table_create(void) {
    SymbolTable* symtab = malloc(sizeof(SymbolTable));
    if (!symtab) return NULL;
    
    symtab->scopes = vector_create((void (*)(void*))scope_free);
    symtab->errors = vector_create(free_direct);
    symtab->warnings = vector_create(free_direct);
    
    // 创建全局作用域
    symtab->global_scope = scope_create(SCOPE_GLOBAL, NULL);
    symtab->current_scope = symtab->global_scope;
    vector_push(symtab->scopes, symtab->global_scope);
    
    // 定义内置类型和函数
    symbol_table_define_builtin_types(symtab);
    symbol_table_define_builtin_functions(symtab);
    
    return symtab;
}

// 释放符号表
void symbol_table_free(SymbolTable* symtab) {
    if (!symtab) return;
    
    // scopes vector 会自动释放所有作用域
    vector_free(symtab->scopes);
    vector_free(symtab->errors);
    vector_free(symtab->warnings);
    free(symtab);
}

// 创建符号
Symbol* symbol_create(SymbolType type, const char* name, Node* node, Type* symbol_type) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) return NULL;
    
    symbol->name = name ? xstrdup(name) : NULL;
    symbol->type = type;
    symbol->node = node;
    symbol->symbol_type = symbol_type;
    symbol->scope = NULL;
    symbol->visibility = VISIBILITY_PRIVATE;
    
    // 初始化特定信息
    switch (type) {
        case SYMBOL_VARIABLE:
            symbol->variable.is_mutable = false;
            symbol->variable.is_constant = false;
            break;
            
        case SYMBOL_FUNCTION:
            symbol->function.is_async = false;
            symbol->function.generic_params = vector_create(free_direct);
            symbol->function.param_types = vector_create((void (*)(void*))type_free);
            symbol->function.return_type = NULL;
            break;
            
        case SYMBOL_STRUCT:
            symbol->structure.generic_params = vector_create(free_direct);
            symbol->structure.fields = vector_create(free_direct);
            break;
            
        case SYMBOL_ENUM:
            symbol->enumeration.variants = vector_create(free_direct);
            break;
            
        case SYMBOL_TYPE_PARAM:
            symbol->type_param.constraint = NULL;
            break;
            
        default:
            break;
    }
    
    return symbol;
}

// 释放符号
void symbol_free(Symbol* symbol) {
    if (!symbol) return;
    
    free(symbol->name);
    
    if (symbol->symbol_type) {
        type_free(symbol->symbol_type);
    }
    
    switch (symbol->type) {
        case SYMBOL_FUNCTION:
            vector_free(symbol->function.generic_params);
            vector_free(symbol->function.param_types);
            if (symbol->function.return_type) {
                type_free(symbol->function.return_type);
            }
            break;
            
        case SYMBOL_STRUCT:
            vector_free(symbol->structure.generic_params);
            vector_free(symbol->structure.fields);
            break;
            
        case SYMBOL_ENUM:
            vector_free(symbol->enumeration.variants);
            break;
            
        case SYMBOL_TYPE_PARAM:
            if (symbol->type_param.constraint) {
                type_free(symbol->type_param.constraint);
            }
            break;
            
        default:
            break;
    }
    
    free(symbol);
}

// 定义符号
bool symbol_table_define(SymbolTable* symtab, Symbol* symbol) {
    if (!symtab || !symbol || !symbol->name) {
        return false;
    }
    
    // 检查是否在当前作用域已定义
    if (scope_lookup_local(symtab->current_scope, symbol->name)) {
        symbol_table_add_error(symtab, 
            "Symbol '%s' is already defined in this scope", 
            symbol->name, symbol->node ? symbol->node->line : 0, 
            symbol->node ? symbol->node->column : 0);
        return false;
    }
    
    symbol->scope = symtab->current_scope;
    return scope_define(symtab->current_scope, symbol);
}

// 解析符号（从当前作用域向上查找）
Symbol* symbol_table_resolve(SymbolTable* symtab, const char* name) {
    if (!symtab || !name) return NULL;
    
    Scope* current = symtab->current_scope;
    while (current) {
        Symbol* symbol = scope_lookup_local(current, name);
        if (symbol) {
            return symbol;
        }
        current = current->parent;
    }
    
    return NULL;
}

// 只在当前作用域解析符号
Symbol* symbol_table_resolve_current_scope(SymbolTable* symtab, const char* name) {
    if (!symtab || !name) return NULL;
    return scope_lookup_local(symtab->current_scope, name);
}

// 在全局作用域解析符号
Symbol* symbol_table_resolve_global(SymbolTable* symtab, const char* name) {
    if (!symtab || !name) return NULL;
    return scope_lookup_local(symtab->global_scope, name);
}

// 检查符号是否已定义
bool symbol_table_is_defined(SymbolTable* symtab, const char* name) {
    return symbol_table_resolve(symtab, name) != NULL;
}

bool symbol_table_is_defined_in_current_scope(SymbolTable* symtab, const char* name) {
    return symbol_table_resolve_current_scope(symtab, name) != NULL;
}

// 进入新作用域
void symbol_table_enter_scope(SymbolTable* symtab, ScopeType type) {
    if (!symtab) return;
    
    Scope* new_scope = scope_create(type, symtab->current_scope);
    symtab->current_scope = new_scope;
    vector_push(symtab->scopes, new_scope);
}

// 退出当前作用域
void symbol_table_exit_scope(SymbolTable* symtab) {
    if (!symtab || !symtab->current_scope || 
        symtab->current_scope->type == SCOPE_GLOBAL) {
        return;
    }
    
    symtab->current_scope = symtab->current_scope->parent;
}

// 定义类型符号
Symbol* symbol_table_define_type(SymbolTable* symtab, const char* name, Type* type, Node* node) {
    Symbol* symbol = symbol_create(SYMBOL_BUILTIN_TYPE, name, node, type);
    if (!symbol) return NULL;
    
    if (symbol_table_define(symtab, symbol)) {
        return symbol;
    }
    
    symbol_free(symbol);
    return NULL;
}

// 解析类型符号
Symbol* symbol_table_resolve_type(SymbolTable* symtab, const char* name) {
    // 类型符号通常在全局作用域定义
    return symbol_table_resolve_global(symtab, name);
}

// 错误处理
void symbol_table_add_error(SymbolTable* symtab, const char* message, int line, int column) {
    if (!symtab || !message) return;
    
    StringBuilder* sb = sb_create();
    sb_append_format(sb, "[Line %d, Column %d] Semantic Error: %s", line, column, message);
    
    char* error_msg = sb_to_string(sb);
    vector_push(symtab->errors, error_msg);
    sb_free(sb);
}

void symbol_table_add_warning(SymbolTable* symtab, const char* message, int line, int column) {
    if (!symtab || !message) return;
    
    StringBuilder* sb = sb_create();
    sb_append_format(sb, "[Line %d, Column %d] Semantic Warning: %s", line, column, message);
    
    char* warning_msg = sb_to_string(sb);
    vector_push(symtab->warnings, warning_msg);
    sb_free(sb);
}

bool symbol_table_has_errors(const SymbolTable* symtab) {
    return symtab && !vector_is_empty(symtab->errors);
}

void symbol_table_print_errors(const SymbolTable* symtab) {
    if (!symtab || vector_is_empty(symtab->errors)) {
        return;
    }
    
    printf("Semantic Errors:\n");
    for (size_t i = 0; i < vector_size(symtab->errors); i++) {
        char* error = vector_get(symtab->errors, i);
        printf("  %s\n", error);
    }
}

void symbol_table_print_warnings(const SymbolTable* symtab) {
    if (!symtab || vector_is_empty(symtab->warnings)) {
        return;
    }
    
    printf("Semantic Warnings:\n");
    for (size_t i = 0; i < vector_size(symtab->warnings); i++) {
        char* warning = vector_get(symtab->warnings, i);
        printf("  %s\n", warning);
    }
}

// 定义内置类型
void symbol_table_define_builtin_types(SymbolTable* symtab) {
    if (!symtab) return;
    
    // 基本类型
    symbol_table_define_type(symtab, "int8", type_create_basic(TYPE_INT8, "int8"), NULL);
    symbol_table_define_type(symtab, "int16", type_create_basic(TYPE_INT16, "int16"), NULL);
    symbol_table_define_type(symtab, "int32", type_create_basic(TYPE_INT32, "int32"), NULL);
    symbol_table_define_type(symtab, "int64", type_create_basic(TYPE_INT64, "int64"), NULL);
    symbol_table_define_type(symtab, "uint8", type_create_basic(TYPE_UINT8, "uint8"), NULL);
    symbol_table_define_type(symtab, "uint16", type_create_basic(TYPE_UINT16, "uint16"), NULL);
    symbol_table_define_type(symtab, "uint32", type_create_basic(TYPE_UINT32, "uint32"), NULL);
    symbol_table_define_type(symtab, "uint64", type_create_basic(TYPE_UINT64, "uint64"), NULL);
    symbol_table_define_type(symtab, "float32", type_create_basic(TYPE_FLOAT32, "float32"), NULL);
    symbol_table_define_type(symtab, "float64", type_create_basic(TYPE_FLOAT64, "float64"), NULL);
    symbol_table_define_type(symtab, "bool", type_create_basic(TYPE_BOOL, "bool"), NULL);
    symbol_table_define_type(symtab, "char", type_create_basic(TYPE_CHAR, "char"), NULL);
    symbol_table_define_type(symtab, "string", type_create_basic(TYPE_STRING, "string"), NULL);
    symbol_table_define_type(symtab, "void", type_create_basic(TYPE_VOID, "void"), NULL);
}

// 定义内置函数
void symbol_table_define_builtin_functions(SymbolTable* symtab) {
    if (!symtab) return;
    
    // 示例：定义print函数
    Symbol* print_symbol = symbol_create(SYMBOL_FUNCTION, "print", NULL, NULL);
    if (print_symbol) {
        print_symbol->visibility = VISIBILITY_PUBLIC;
        
        // 设置参数类型：print(value: string) -> void
        Vector* param_types = vector_create((void (*)(void*))type_free);
        vector_push(param_types, type_create_basic(TYPE_STRING, "string"));
        
        print_symbol->function.param_types = param_types;
        print_symbol->function.return_type = type_create_basic(TYPE_VOID, "void");
        
        symbol_table_define(symtab, print_symbol);
    }
    
    // 可以继续添加其他内置函数...
}

// 工具函数
const char* symbol_type_to_string(SymbolType type) {
    static const char* names[] = {
        "VARIABLE", "FUNCTION", "STRUCT", "ENUM", 
        "ENUM_VARIANT", "TRAIT", "MODULE", "TYPE_PARAM", "BUILTIN_TYPE"
    };
    
    if (type >= SYMBOL_VARIABLE && type <= SYMBOL_BUILTIN_TYPE) {
        return names[type];
    }
    
    return "UNKNOWN";
}

const char* scope_type_to_string(ScopeType type) {
    static const char* names[] = {
        "GLOBAL", "MODULE", "FUNCTION", "BLOCK", 
        "LOOP", "STRUCT", "IMPL"
    };
    
    if (type >= SCOPE_GLOBAL && type <= SCOPE_IMPL) {
        return names[type];
    }
    
    return "UNKNOWN";
}

void symbol_print(const Symbol* symbol) {
    if (!symbol) {
        printf("Symbol: NULL\n");
        return;
    }
    
    printf("Symbol{name: %s, type: %s, visibility: %s", 
           symbol->name ? symbol->name : "NULL",
           symbol_type_to_string(symbol->type),
           symbol->visibility == VISIBILITY_PUBLIC ? "public" : 
           symbol->visibility == VISIBILITY_MODULE ? "module" : "private");
    
    if (symbol->symbol_type) {
        printf(", symbol_type: ");
        type_print(symbol->symbol_type);
    }
    
    printf("}\n");
}

void scope_print(const Scope* scope, int indent) {
    if (!scope) return;
    
    // 缩进
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    printf("Scope{type: %s", scope_type_to_string(scope->type));
    
    // 打印符号数量
    if (scope->symbols) {
        printf(", symbols: %zu", hash_map_size(scope->symbols));
    }
    
    printf("}\n");
    
    // 递归打印子作用域
    for (size_t i = 0; i < vector_size(scope->children); i++) {
        Scope* child = vector_get(scope->children, i);
        scope_print(child, indent + 1);
    }
}