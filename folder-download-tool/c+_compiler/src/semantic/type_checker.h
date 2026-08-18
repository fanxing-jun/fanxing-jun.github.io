#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "symbol_table.h"
#include "../ast/ast.h"

typedef struct TypeChecker {
    SymbolTable* symbol_table;     // 符号表
    Vector* errors;                // 类型错误
    Vector* warnings;              // 类型警告
    bool in_loop;                  // 是否在循环中
    bool in_function;              // 是否在函数中
    Type* current_return_type;     // 当前函数的返回类型
    Node* current_function;        // 当前函数节点
} TypeChecker;

// ===== 创建和销毁 =====
TypeChecker* type_checker_create(SymbolTable* symbol_table);
void type_checker_free(TypeChecker* tc);

// ===== 主要类型检查函数 =====
bool type_check_program(TypeChecker* tc, Node* program);
bool type_check_node(TypeChecker* tc, Node* node);

// ===== 声明类型检查 =====
bool type_check_function_decl(TypeChecker* tc, Node* node);
bool type_check_variable_decl(TypeChecker* tc, Node* node);
bool type_check_struct_decl(TypeChecker* tc, Node* node);
bool type_check_enum_decl(TypeChecker* tc, Node* node);

// ===== 语句类型检查 =====
bool type_check_block_stmt(TypeChecker* tc, Node* node);
bool type_check_expr_stmt(TypeChecker* tc, Node* node);
bool type_check_if_stmt(TypeChecker* tc, Node* node);
bool type_check_for_stmt(TypeChecker* tc, Node* node);
bool type_check_while_stmt(TypeChecker* tc, Node* node);
bool type_check_match_stmt(TypeChecker* tc, Node* node);
bool type_check_return_stmt(TypeChecker* tc, Node* node);
bool type_check_break_stmt(TypeChecker* tc, Node* node);
bool type_check_continue_stmt(TypeChecker* tc, Node* node);

// ===== 表达式类型检查 =====
Type* type_check_expression(TypeChecker* tc, Node* node);
Type* type_check_binary_expr(TypeChecker* tc, Node* node);
Type* type_check_unary_expr(TypeChecker* tc, Node* node);
Type* type_check_call_expr(TypeChecker* tc, Node* node);
Type* type_check_method_call_expr(TypeChecker* tc, Node* node);
Type* type_check_field_access_expr(TypeChecker* tc, Node* node);
Type* type_check_array_access_expr(TypeChecker* tc, Node* node);
Type* type_check_literal_expr(TypeChecker* tc, Node* node);
Type* type_check_identifier_expr(TypeChecker* tc, Node* node);
Type* type_check_if_expr(TypeChecker* tc, Node* node);
Type* type_check_match_expr(TypeChecker* tc, Node* node);
Type* type_check_array_expr(TypeChecker* tc, Node* node);
Type* type_check_struct_expr(TypeChecker* tc, Node* node);
Type* type_check_closure_expr(TypeChecker* tc, Node* node);
Type* type_check_await_expr(TypeChecker* tc, Node* node);

// ===== 模式类型检查 =====
bool type_check_pattern(TypeChecker* tc, Node* pattern, Type* expected_type);
Type* type_check_pattern_and_bind(TypeChecker* tc, Node* pattern, Type* expected_type);

// ===== 类型推断 =====
Type* infer_expression_type(TypeChecker* tc, Node* node);
Type* infer_binary_expression_type(TypeChecker* tc, TokenType operator, Type* left, Type* right);
Type* infer_unary_expression_type(TypeChecker* tc, TokenType operator, Type* operand);

// ===== 类型兼容性检查 =====
bool types_are_compatible(TypeChecker* tc, Type* actual, Type* expected);
bool types_are_assignable(TypeChecker* tc, Type* target, Type* source);
bool types_are_equal(Type* a, Type* b);
bool is_numeric_type(Type* type);
bool is_integer_type(Type* type);
bool is_float_type(Type* type);
bool is_boolean_type(Type* type);

// ===== 错误处理 =====
void type_error(TypeChecker* tc, const char* message, int line, int column);
void type_error_with_types(TypeChecker* tc, const char* message, Type* actual, Type* expected, int line, int column);
void type_warning(TypeChecker* tc, const char* message, int line, int column);
bool type_checker_has_errors(const TypeChecker* tc);
void type_checker_print_errors(const TypeChecker* tc);
void type_checker_print_warnings(const TypeChecker* tc);

// ===== 工具函数 =====
Type* get_common_type(TypeChecker* tc, Type* a, Type* b);
Type* resolve_type_annotation(TypeChecker* tc, Node* type_annotation);
const char* type_to_string(Type* type);

#endif