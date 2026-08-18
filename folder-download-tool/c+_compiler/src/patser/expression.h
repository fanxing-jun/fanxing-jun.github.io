#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "parser.h"

// 表达式解析函数
Node* parse_expression(Parser* parser);
Node* parse_assignment(Parser* parser);
Node* parse_conditional(Parser* parser);
Node* parse_logical_or(Parser* parser);
Node* parse_logical_and(Parser* parser);
Node* parse_equality(Parser* parser);
Node* parse_comparison(Parser* parser);
Node* parse_term(Parser* parser);
Node* parse_factor(Parser* parser);
Node* parse_unary(Parser* parser);
Node* parse_call(Parser* parser);
Node* parse_primary(Parser* parser);

// 特定表达式解析
Node* parse_literal(Parser* parser);
Node* parse_grouped_expression(Parser* parser);
Node* parse_if_expression(Parser* parser);
Node* parse_match_expression(Parser* parser);
Node* parse_array_expression(Parser* parser);
Node* parse_struct_expression(Parser* parser);
Node* parse_tuple_expression(Parser* parser);
Node* parse_closure_expression(Parser* parser);

// 工具函数
Node* parse_argument_list(Parser* parser);
Node* parse_field_initializer(Parser* parser);

#endif