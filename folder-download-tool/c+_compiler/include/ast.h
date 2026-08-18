#ifndef AST_H
#define AST_H

#include "tokens.h"
#include "../utils/vector.h"
#include <stdbool.h>

// ===== 前置声明 =====
typedef struct Node Node;
typedef struct Type Type;
typedef struct Symbol Symbol;

// ===== 类型系统 =====
typedef enum {
    TYPE_VOID,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_FLOAT32,
    TYPE_FLOAT64,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_SLICE,
    TYPE_TUPLE,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_FUNCTION,
    TYPE_GENERIC,
    TYPE_UNKNOWN
} TypeKind;

typedef struct Type {
    TypeKind kind;
    char* name;  // 类型名称（用于错误消息）
    
    // 类型参数（用于泛型、数组、函数等）
    union {
        // 数组类型: [T; N]
        struct {
            struct Type* element_type;
            int array_size;  // -1 表示动态大小
        } array;
        
        // 切片类型: []T
        struct {
            struct Type* element_type;
        } slice;
        
        // 元组类型: (T1, T2, ...)
        struct {
            Vector* element_types;  // Vector<Type*>
        } tuple;
        
        // 函数类型: (T1, T2) -> T3
        struct {
            Vector* param_types;    // Vector<Type*>
            struct Type* return_type;
        } function;
        
        // 泛型类型
        struct {
            char* generic_name;
            Vector* type_args;      // Vector<Type*>
        } generic;
        
        // 结构体/枚举类型
        struct {
            char* type_name;
            Symbol* symbol;         // 指向类型定义
        } named;
    };
} Type;

// ===== AST 节点类型 =====
typedef enum {
    // 声明
    NODE_PROGRAM,
    NODE_FUNCTION_DECL,
    NODE_VAR_DECL,
    NODE_STRUCT_DECL,
    NODE_ENUM_DECL,
    NODE_ENUM_VARIANT,
    NODE_IMPL_BLOCK,
    NODE_TRAIT_DECL,
    NODE_MODULE_DECL,
    NODE_IMPORT_DECL,
    
    // 语句
    NODE_BLOCK_STMT,
    NODE_EXPR_STMT,
    NODE_IF_STMT,
    NODE_FOR_STMT,
    NODE_WHILE_STMT,
    NODE_LOOP_STMT,
    NODE_MATCH_STMT,
    NODE_RETURN_STMT,
    NODE_BREAK_STMT,
    NODE_CONTINUE_STMT,
    
    // 表达式
    NODE_BINARY_EXPR,
    NODE_UNARY_EXPR,
    NODE_CALL_EXPR,
    NODE_METHOD_CALL_EXPR,
    NODE_FIELD_ACCESS_EXPR,
    NODE_ARRAY_ACCESS_EXPR,
    NODE_LITERAL_EXPR,
    NODE_IDENTIFIER_EXPR,
    NODE_GROUP_EXPR,
    NODE_IF_EXPR,
    NODE_MATCH_EXPR,
    NODE_ARRAY_EXPR,
    NODE_STRUCT_EXPR,
    NODE_TUPLE_EXPR,
    NODE_CLOSURE_EXPR,
    NODE_ASYNC_EXPR,
    NODE_AWAIT_EXPR,
    
    // 模式匹配
    NODE_PATTERN_IDENT,
    NODE_PATTERN_LITERAL,
    NODE_PATTERN_WILDCARD,
    NODE_PATTERN_STRUCT,
    NODE_PATTERN_TUPLE,
    NODE_PATTERN_OR,
    
    // 类型
    NODE_TYPE_ANNOTATION,
    
    // 其他
    NODE_PARAM,
    NODE_ARGUMENT,
    NODE_FIELD_DEF,
    NODE_MATCH_ARM
} NodeType;

// ===== AST 节点结构 =====
struct Node {
    NodeType type;
    int line;
    int column;
    Type* node_type;  // 类型推断结果
    
    // 链表指针（用于同一层级的节点）
    struct Node* next;
    
    union {
        // === 声明节点 ===
        
        // 程序根节点: NODE_PROGRAM
        struct {
            Vector* declarations;  // Vector<Node*> - 函数、结构体等声明
        } program;
        
        // 函数声明: NODE_FUNCTION_DECL
        struct {
            char* name;
            bool is_public;
            bool is_async;
            Vector* generic_params;  // Vector<Node*> - 泛型参数
            Vector* params;          // Vector<Node*> - 参数列表
            Node* return_type;       // Node* - 返回类型注解
            Node* body;              // Node* - 函数体
        } func_decl;
        
        // 变量声明: NODE_VAR_DECL
        struct {
            char* name;
            bool is_mutable;
            bool is_public;
            Node* type_annotation;   // Node* - 类型注解（可选）
            Node* value;             // Node* - 初始值（可选）
        } var_decl;
        
        // 结构体声明: NODE_STRUCT_DECL
        struct {
            char* name;
            bool is_public;
            Vector* generic_params;  // Vector<Node*>
            Vector* fields;          // Vector<Node*> - 字段定义
        } struct_decl;
        
        // 枚举声明: NODE_ENUM_DECL
        struct {
            char* name;
            bool is_public;
            Vector* variants;        // Vector<Node*> - 枚举变体
        } enum_decl;
        
        // 枚举变体: NODE_ENUM_VARIANT
        struct {
            char* name;
            Vector* associated_data; // Vector<Node*> - 关联数据类型（可选）
        } enum_variant;
        
        // impl 块: NODE_IMPL_BLOCK
        struct {
            Vector* generic_params;  // Vector<Node*>
            Node* target_type;       // Node* - 目标类型
            Vector* functions;       // Vector<Node*> - 实现的方法
        } impl_block;
        
        // === 语句节点 ===
        
        // 块语句: NODE_BLOCK_STMT
        struct {
            Vector* statements;      // Vector<Node*>
        } block_stmt;
        
        // 表达式语句: NODE_EXPR_STMT
        struct {
            Node* expression;        // Node*
        } expr_stmt;
        
        // if 语句: NODE_IF_STMT
        struct {
            Node* condition;         // Node*
            Node* then_branch;       // Node*
            Node* else_branch;       // Node*（可选）
        } if_stmt;
        
        // for 循环: NODE_FOR_STMT
        struct {
            Node* pattern;           // Node* - 模式（let x）
            Node* iterable;          // Node* - 可迭代对象
            Node* body;              // Node* - 循环体
        } for_stmt;
        
        // while 循环: NODE_WHILE_STMT
        struct {
            Node* condition;         // Node*
            Node* body;              // Node*
        } while_stmt;
        
        // match 语句: NODE_MATCH_STMT
        struct {
            Node* subject;           // Node* - 匹配主体
            Vector* arms;            // Vector<Node*> - 匹配臂
        } match_stmt;
        
        // 返回语句: NODE_RETURN_STMT
        struct {
            Node* value;             // Node*（可选）
        } return_stmt;
        
        // === 表达式节点 ===
        
        // 二元表达式: NODE_BINARY_EXPR
        struct {
            TokenType operator;
            Node* left;
            Node* right;
        } binary_expr;
        
        // 一元表达式: NODE_UNARY_EXPR
        struct {
            TokenType operator;
            Node* operand;
        } unary_expr;
        
        // 函数调用: NODE_CALL_EXPR
        struct {
            Node* callee;            // Node*
            Vector* arguments;       // Vector<Node*>
        } call_expr;
        
        // 方法调用: NODE_METHOD_CALL_EXPR
        struct {
            Node* receiver;          // Node*
            char* method_name;       // 方法名
            Vector* arguments;       // Vector<Node*>
        } method_call_expr;
        
        // 字段访问: NODE_FIELD_ACCESS_EXPR
        struct {
            Node* object;            // Node*
            char* field_name;        // 字段名
        } field_access_expr;
        
        // 数组访问: NODE_ARRAY_ACCESS_EXPR
        struct {
            Node* array;             // Node*
            Node* index;             // Node*
        } array_access_expr;
        
        // 字面量: NODE_LITERAL_EXPR
        struct {
            Token literal_token;
        } literal_expr;
        
        // 标识符: NODE_IDENTIFIER_EXPR
        struct {
            char* name;
        } identifier_expr;
        
        // 分组表达式: NODE_GROUP_EXPR
        struct {
            Node* expression;        // Node*
        } group_expr;
        
        // if 表达式: NODE_IF_EXPR
        struct {
            Node* condition;         // Node*
            Node* then_branch;       // Node*
            Node* else_branch;       // Node*
        } if_expr;
        
        // match 表达式: NODE_MATCH_EXPR
        struct {
            Node* subject;           // Node*
            Vector* arms;            // Vector<Node*>
        } match_expr;
        
        // 数组表达式: NODE_ARRAY_EXPR
        struct {
            Vector* elements;        // Vector<Node*>
        } array_expr;
        
        // 结构体表达式: NODE_STRUCT_EXPR
        struct {
            char* struct_name;       // 结构体名
            Vector* fields;          // Vector<Node*> - 字段初始化
        } struct_expr;
        
        // 元组表达式: NODE_TUPLE_EXPR
        struct {
            Vector* elements;        // Vector<Node*>
        } tuple_expr;
        
        // 闭包表达式: NODE_CLOSURE_EXPR
        struct {
            Vector* params;          // Vector<Node*>
            Node* return_type;       // Node*（可选）
            Node* body;              // Node*
            bool is_async;
        } closure_expr;
        
        // await 表达式: NODE_AWAIT_EXPR
        struct {
            Node* expression;        // Node*
        } await_expr;
        
        // === 模式匹配节点 ===
        
        // 标识符模式: NODE_PATTERN_IDENT
        struct {
            char* name;
            bool is_mutable;         // 是否可变绑定
            Node* subpattern;        // Node* - 子模式（可选，用于解构）
        } pattern_ident;
        
        // 字面量模式: NODE_PATTERN_LITERAL
        struct {
            Node* literal;           // Node* - 字面量表达式
        } pattern_literal;
        
        // 结构体模式: NODE_PATTERN_STRUCT
        struct {
            char* struct_name;       // 结构体名
            Vector* field_patterns;  // Vector<Node*> - 字段模式
        } pattern_struct;
        
        // 元组模式: NODE_PATTERN_TUPLE
        struct {
            Vector* element_patterns; // Vector<Node*>
        } pattern_tuple;
        
        // 或模式: NODE_PATTERN_OR
        struct {
            Vector* patterns;        // Vector<Node*> - 多个模式
        } pattern_or;
        
        // === 其他节点 ===
        
        // 参数: NODE_PARAM
        struct {
            char* name;
            Node* type_annotation;   // Node*
            bool is_mutable;
        } param;
        
        // 字段定义: NODE_FIELD_DEF
        struct {
            char* name;
            Node* type_annotation;   // Node*
            bool is_public;
        } field_def;
        
        // 匹配臂: NODE_MATCH_ARM
        struct {
            Node* pattern;           // Node*
            Node* guard;             // Node* - 守卫条件（可选）
            Node* body;              // Node*
        } match_arm;
        
        // 类型注解: NODE_TYPE_ANNOTATION
        struct {
            Type* type;              // Type*
        } type_annotation;
    };
};

// ===== 创建节点的函数声明 =====
Node* ast_program(Vector* declarations, int line, int column);
Node* ast_function_decl(char* name, bool is_public, bool is_async, 
                       Vector* generic_params, Vector* params, 
                       Node* return_type, Node* body, int line, int column);
Node* ast_var_decl(char* name, bool is_mutable, bool is_public,
                  Node* type_annotation, Node* value, int line, int column);
Node* ast_struct_decl(char* name, bool is_public, Vector* generic_params, 
                     Vector* fields, int line, int column);
Node* ast_enum_decl(char* name, bool is_public, Vector* variants, int line, int column);
Node* ast_enum_variant(char* name, Vector* associated_data, int line, int column);

// 语句节点
Node* ast_block_stmt(Vector* statements, int line, int column);
Node* ast_expr_stmt(Node* expression, int line, int column);
Node* ast_if_stmt(Node* condition, Node* then_branch, Node* else_branch, int line, int column);
Node* ast_for_stmt(Node* pattern, Node* iterable, Node* body, int line, int column);
Node* ast_while_stmt(Node* condition, Node* body, int line, int column);
Node* ast_match_stmt(Node* subject, Vector* arms, int line, int column);
Node* ast_return_stmt(Node* value, int line, int column);
Node* ast_break_stmt(int line, int column);
Node* ast_continue_stmt(int line, int column);

// 表达式节点
Node* ast_binary_expr(TokenType operator, Node* left, Node* right, int line, int column);
Node* ast_unary_expr(TokenType operator, Node* operand, int line, int column);
Node* ast_call_expr(Node* callee, Vector* arguments, int line, int column);
Node* ast_method_call_expr(Node* receiver, char* method_name, Vector* arguments, int line, int column);
Node* ast_field_access_expr(Node* object, char* field_name, int line, int column);
Node* ast_array_access_expr(Node* array, Node* index, int line, int column);
Node* ast_literal_expr(Token literal_token, int line, int column);
Node* ast_identifier_expr(char* name, int line, int column);
Node* ast_group_expr(Node* expression, int line, int column);
Node* ast_if_expr(Node* condition, Node* then_branch, Node* else_branch, int line, int column);
Node* ast_match_expr(Node* subject, Vector* arms, int line, int column);
Node* ast_array_expr(Vector* elements, int line, int column);
Node* ast_struct_expr(char* struct_name, Vector* fields, int line, int column);
Node* ast_tuple_expr(Vector* elements, int line, int column);
Node* ast_closure_expr(Vector* params, Node* return_type, Node* body, bool is_async, int line, int column);
Node* ast_await_expr(Node* expression, int line, int column);

// 模式节点
Node* ast_pattern_ident(char* name, bool is_mutable, Node* subpattern, int line, int column);
Node* ast_pattern_literal(Node* literal, int line, int column);
Node* ast_pattern_wildcard(int line, int column);
Node* ast_pattern_struct(char* struct_name, Vector* field_patterns, int line, int column);
Node* ast_pattern_tuple(Vector* element_patterns, int line, int column);
Node* ast_pattern_or(Vector* patterns, int line, int column);

// 其他节点
Node* ast_param(char* name, Node* type_annotation, bool is_mutable, int line, int column);
Node* ast_field_def(char* name, Node* type_annotation, bool is_public, int line, int column);
Node* ast_match_arm(Node* pattern, Node* guard, Node* body, int line, int column);
Node* ast_type_annotation(Type* type, int line, int column);

// 类型创建函数
Type* type_create_basic(TypeKind kind, const char* name);
Type* type_create_array(Type* element_type, int size);
Type* type_create_slice(Type* element_type);
Type* type_create_tuple(Vector* element_types);
Type* type_create_function(Vector* param_types, Type* return_type);
Type* type_create_generic(const char* name, Vector* type_args);
Type* type_create_named(const char* name, Symbol* symbol);

// 节点释放函数
void ast_free(Node* node);
void type_free(Type* type);

#endif