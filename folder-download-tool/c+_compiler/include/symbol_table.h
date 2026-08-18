#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../ast/ast.h"
#include "../utils/vector.h"
#include "../utils/hash_map.h"

// ===== 符号类型 =====
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_STRUCT,
    SYMBOL_ENUM,
    SYMBOL_ENUM_VARIANT,
    SYMBOL_TRAIT,
    SYMBOL_MODULE,
    SYMBOL_TYPE_PARAM,  // 泛型类型参数
    SYMBOL_BUILTIN_TYPE // 内置类型
} SymbolType;

// ===== 符号可见性 =====
typedef enum {
    VISIBILITY_PRIVATE,
    VISIBILITY_PUBLIC,
    VISIBILITY_MODULE  // 模块内可见
} SymbolVisibility;

// ===== 符号定义 =====
typedef struct Symbol {
    char* name;                    // 符号名称
    SymbolType type;               // 符号类型
    SymbolVisibility visibility;   // 可见性
    Node* node;                    // 对应的AST节点
    Type* symbol_type;             // 符号的类型信息
    struct Scope* scope;           // 所属作用域
    
    // 符号特定信息
    union {
        // 变量符号
        struct {
            bool is_mutable;       // 是否可变
            bool is_constant;      // 是否是常量
        } variable;
        
        // 函数符号
        struct {
            bool is_async;         // 是否是异步函数
            Vector* generic_params; // 泛型参数 Vector<Symbol*>
            Vector* param_types;    // 参数类型 Vector<Type*>
            Type* return_type;     // 返回类型
        } function;
        
        // 结构体符号
        struct {
            Vector* generic_params; // 泛型参数
            Vector* fields;         // 字段信息 Vector<Symbol*>
        } structure;
        
        // 枚举符号
        struct {
            Vector* variants;      // 枚举变体 Vector<Symbol*>
        } enumeration;
        
        // 类型参数符号（泛型）
        struct {
            Type* constraint;      // 类型约束（可选）
        } type_param;
    };
} Symbol;

// ===== 作用域类型 =====
typedef enum {
    SCOPE_GLOBAL,
    SCOPE_MODULE,
    SCOPE_FUNCTION,
    SCOPE_BLOCK,
    SCOPE_LOOP,
    SCOPE_STRUCT,      // 结构体作用域（用于字段）
    SCOPE_IMPL         // impl块作用域
} ScopeType;

// ===== 作用域定义 =====
typedef struct Scope {
    ScopeType type;                // 作用域类型
    struct Scope* parent;          // 父作用域
    HashMap* symbols;              // 符号表 HashMap<char*, Symbol*>
    Vector* children;              // 子作用域 Vector<Scope*>
    
    // 作用域特定信息
    union {
        // 函数作用域
        struct {
            Symbol* function_symbol; // 对应的函数符号
            bool has_return;        // 是否包含return语句
        } function_scope;
        
        // 循环作用域
        struct {
            bool can_break;         // 是否允许break
            bool can_continue;      // 是否允许continue
        } loop_scope;
        
        // 结构体作用域
        struct {
            Symbol* struct_symbol;  // 对应的结构体符号
        } struct_scope;
        
        // 模块作用域
        struct {
            char* module_name;      // 模块名称
        } module_scope;
    };
} Scope;

// ===== 符号表管理器 =====
typedef struct {
    Scope* global_scope;           // 全局作用域
    Scope* current_scope;          // 当前作用域
    Vector* scopes;                // 所有作用域（用于内存管理）
    Vector* errors;                // 错误列表
    Vector* warnings;              // 警告列表
} SymbolTable;

// ===== 符号表创建和销毁 =====
SymbolTable* symbol_table_create(void);
void symbol_table_free(SymbolTable* symtab);

// ===== 作用域管理 =====
Scope* scope_create(ScopeType type, Scope* parent);
void scope_free(Scope* scope);
void symbol_table_enter_scope(SymbolTable* symtab, ScopeType type);
void symbol_table_exit_scope(SymbolTable* symtab);

// ===== 符号操作 =====
Symbol* symbol_create(SymbolType type, const char* name, Node* node, Type* symbol_type);
void symbol_free(Symbol* symbol);

bool symbol_table_define(SymbolTable* symtab, Symbol* symbol);
Symbol* symbol_table_resolve(SymbolTable* symtab, const char* name);
Symbol* symbol_table_resolve_current_scope(SymbolTable* symtab, const char* name);
Symbol* symbol_table_resolve_global(SymbolTable* symtab, const char* name);

// ===== 符号查询 =====
bool symbol_table_is_defined(SymbolTable* symtab, const char* name);
bool symbol_table_is_defined_in_current_scope(SymbolTable* symtab, const char* name);

// ===== 错误处理 =====
void symbol_table_add_error(SymbolTable* symtab, const char* message, int line, int column);
void symbol_table_add_warning(SymbolTable* symtab, const char* message, int line, int column);
bool symbol_table_has_errors(const SymbolTable* symtab);
void symbol_table_print_errors(const SymbolTable* symtab);
void symbol_table_print_warnings(const SymbolTable* symtab);

// ===== 类型符号操作 =====
Symbol* symbol_table_define_type(SymbolTable* symtab, const char* name, Type* type, Node* node);
Symbol* symbol_table_resolve_type(SymbolTable* symtab, const char* name);

// ===== 内置类型和函数 =====
void symbol_table_define_builtin_types(SymbolTable* symtab);
void symbol_table_define_builtin_functions(SymbolTable* symtab);

// ===== 工具函数 =====
const char* symbol_type_to_string(SymbolType type);
const char* scope_type_to_string(ScopeType type);
void symbol_print(const Symbol* symbol);
void scope_print(const Scope* scope, int indent);

#endif