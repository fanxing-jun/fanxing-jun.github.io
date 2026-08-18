#ifndef AST_UTILS_H
#define AST_UTILS_H

#include "ast.h"

// 节点信息查询
const char* node_type_to_string(NodeType type);
bool node_is_expression(NodeType type);
bool node_is_statement(NodeType type);
bool node_is_declaration(NodeType type);
bool node_is_pattern(NodeType type);

// 类型工具函数
const char* type_kind_to_string(TypeKind kind);
bool type_equals(const Type* a, const Type* b);
Type* type_copy(const Type* type);
bool type_is_numeric(const Type* type);
bool type_is_integer(const Type* type);
bool type_is_float(const Type* type);

// 节点遍历
typedef void (*NodeVisitor)(Node* node, void* context);
void ast_traverse(Node* node, NodeVisitor visitor, void* context);

// 调试和打印
void ast_print(Node* node, int indent);
char* ast_to_string(Node* node);
void type_print(const Type* type);

// 实用函数
Node* ast_clone(Node* node);
bool ast_has_errors(Node* node);

#endif