#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast/ast.h"
#include "../semantic/symbol_table.h"
#include "../utils/string_builder.h"

typedef struct CodeGenerator {
    FILE* output;                   // 输出文件
    StringBuilder* code;            // 生成的C代码
    SymbolTable* symbol_table;      // 符号表
    int indent_level;               // 当前缩进级别
    bool in_function;               // 是否在函数中
    bool in_loop;                   // 是否在循环中
    Vector* generated_functions;    // 已生成的函数列表
    Vector* generated_structs;      // 已生成的结构体列表
    Vector* errors;                 // 代码生成错误
    Vector* warnings;               // 代码生成警告
    
    // 运行时库引用
    bool needs_runtime;             // 是否需要运行时库
    HashMap* runtime_functions;     // 需要引用的运行时函数
} CodeGenerator;

// ===== 创建和销毁 =====
CodeGenerator* codegen_create(FILE* output, SymbolTable* symbol_table);
void codegen_free(CodeGenerator* cg);

// ===== 主要代码生成函数 =====
bool codegen_generate(CodeGenerator* cg, Node* program);
bool codegen_generate_node(CodeGenerator* cg, Node* node);

// ===== 声明代码生成 =====
bool codegen_function_decl(CodeGenerator* cg, Node* node);
bool codegen_variable_decl(CodeGenerator* cg, Node* node);
bool codegen_struct_decl(CodeGenerator* cg, Node* node);
bool codegen_enum_decl(CodeGenerator* cg, Node* node);

// ===== 语句代码生成 =====
bool codegen_block_stmt(CodeGenerator* cg, Node* node);
bool codegen_expr_stmt(CodeGenerator* cg, Node* node);
bool codegen_if_stmt(CodeGenerator* cg, Node* node);
bool codegen_for_stmt(CodeGenerator* cg, Node* node);
bool codegen_while_stmt(CodeGenerator* cg, Node* node);
bool codegen_match_stmt(CodeGenerator* cg, Node* node);
bool codegen_return_stmt(CodeGenerator* cg, Node* node);
bool codegen_break_stmt(CodeGenerator* cg, Node* node);
bool codegen_continue_stmt(CodeGenerator* cg, Node* node);

// ===== 表达式代码生成 =====
bool codegen_expression(CodeGenerator* cg, Node* node);
bool codegen_binary_expr(CodeGenerator* cg, Node* node);
bool codegen_unary_expr(CodeGenerator* cg, Node* node);
bool codegen_call_expr(CodeGenerator* cg, Node* node);
bool codegen_method_call_expr(CodeGenerator* cg, Node* node);
bool codegen_field_access_expr(CodeGenerator* cg, Node* node);
bool codegen_array_access_expr(CodeGenerator* cg, Node* node);
bool codegen_literal_expr(CodeGenerator* cg, Node* node);
bool codegen_identifier_expr(CodeGenerator* cg, Node* node);
bool codegen_if_expr(CodeGenerator* cg, Node* node);
bool codegen_array_expr(CodeGenerator* cg, Node* node);
bool codegen_struct_expr(CodeGenerator* cg, Node* node);
bool codegen_closure_expr(CodeGenerator* cg, Node* node);

// ===== 工具函数 =====
void codegen_emit(CodeGenerator* cg, const char* format, ...);
void codegen_emit_line(CodeGenerator* cg, const char* format, ...);
void codegen_indent(CodeGenerator* cg);
void codegen_indent_increase(CodeGenerator* cg);
void codegen_indent_decrease(CodeGenerator* cg);

// ===== 类型映射 =====
const char* codegen_type_to_c_type(Type* type);
const char* codegen_operator_to_c_operator(TokenType operator);

// ===== 错误处理 =====
void codegen_error(CodeGenerator* cg, const char* message, int line, int column);
void codegen_warning(CodeGenerator* cg, const char* message, int line, int column);
bool codegen_has_errors(const CodeGenerator* cg);
void codegen_print_errors(const CodeGenerator* cg);

// ===== 运行时库集成 =====
void codegen_include_runtime(CodeGenerator* cg);
bool codegen_needs_runtime(const CodeGenerator* cg);

#endif