#ifndef TYPE_INFERENCE_H
#define TYPE_INFERENCE_H

#include "type_checker.h"

// 类型推断函数
Type* infer_expression_type(TypeChecker* tc, Node* node);
Type* infer_binary_expression_type(TypeChecker* tc, TokenType operator, Type* left, Type* right);
Type* infer_unary_expression_type(TypeChecker* tc, TokenType operator, Type* operand);
Type* infer_literal_type(TypeChecker* tc, Node* node);
Type* infer_call_expression_type(TypeChecker* tc, Node* node);
Type* infer_identifier_type(TypeChecker* tc, Node* node);

// 约束求解
bool solve_type_constraints(TypeChecker* tc, Vector* constraints);

#endif