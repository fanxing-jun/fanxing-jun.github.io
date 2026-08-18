#ifndef C_EMITTER_H
#define C_EMITTER_H

#include "codegen.h"

// C特定发射器函数
void emitter_emit_function_prologue(CodeGenerator* cg, Node* function_node);
void emitter_emit_function_epilogue(CodeGenerator* cg, Node* function_node);
void emitter_emit_variable_declaration(CodeGenerator* cg, const char* name, Type* type, const char* initializer);
void emitter_emit_struct_definition(CodeGenerator* cg, Node* struct_node);
void emitter_emit_array_initializer(CodeGenerator* cg, Node* array_node);
void emitter_emit_struct_initializer(CodeGenerator* cg, Node* struct_node);

// 控制流发射器
void emitter_emit_if_statement(CodeGenerator* cg, Node* if_node);
void emitter_emit_for_loop(CodeGenerator* cg, Node* for_node);
void emitter_emit_while_loop(CodeGenerator* cg, Node* while_node);
void emitter_emit_switch_statement(CodeGenerator* cg, Node* match_node);

// 表达式发射器
void emitter_emit_binary_expression(CodeGenerator* cg, Node* binary_node);
void emitter_emit_unary_expression(CodeGenerator* cg, Node* unary_node);
void emitter_emit_function_call(CodeGenerator* cg, Node* call_node);
void emitter_emit_literal_value(CodeGenerator* cg, Node* literal_node);

#endif