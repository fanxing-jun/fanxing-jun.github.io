#ifndef DECLARATION_GEN_H
#define DECLARATION_GEN_H

#include "codegen.h"

// 声明生成函数
bool declaration_gen_function(CodeGenerator* cg, Node* node);
bool declaration_gen_variable(CodeGenerator* cg, Node* node);
bool declaration_gen_struct(CodeGenerator* cg, Node* node);
bool declaration_gen_enum(CodeGenerator* cg, Node* node);

#endif