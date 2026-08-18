#ifndef EXPRESSION_GEN_H
#define EXPRESSION_GEN_H

#include "codegen.h"

// 表达式生成函数
bool expression_gen_binary(CodeGenerator* cg, Node* node);
bool expression_gen_unary(CodeGenerator* cg, Node* node);
bool expression_gen_call(CodeGenerator* cg, Node* node);
bool expression_gen_literal(CodeGenerator* cg, Node* node);
bool expression_gen_identifier(CodeGenerator* cg, Node* node);
bool expression_gen_array_access(CodeGenerator* cg, Node* node);
bool expression_gen_field_access(CodeGenerator* cg, Node* node);
bool expression_gen_array(CodeGenerator* cg, Node* node);
bool expression_gen_struct(CodeGenerator* cg, Node* node);

#endif