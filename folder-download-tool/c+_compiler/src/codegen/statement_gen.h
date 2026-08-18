#ifndef STATEMENT_GEN_H
#define STATEMENT_GEN_H

#include "codegen.h"

// 语句生成函数
bool statement_gen_block(CodeGenerator* cg, Node* node);
bool statement_gen_expr(CodeGenerator* cg, Node* node);
bool statement_gen_if(CodeGenerator* cg, Node* node);
bool statement_gen_for(CodeGenerator* cg, Node* node);
bool statement_gen_while(CodeGenerator* cg, Node* node);
bool statement_gen_return(CodeGenerator* cg, Node* node);
bool statement_gen_break(CodeGenerator* cg, Node* node);
bool statement_gen_continue(CodeGenerator* cg, Node* node);

#endif