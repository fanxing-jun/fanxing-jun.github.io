#include "statement_gen.h"
#include "../utils/utils.h"
#include <stdlib.h>

// 生成块语句
bool statement_gen_block(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_BLOCK_STMT) return false;
    
    codegen_emit_line(cg, "{");
    codegen_indent_increase(cg);
    
    // 生成所有语句
    for (size_t i = 0; i < vector_size(node->block_stmt.statements); i++) {
        Node* stmt = vector_get(node->block_stmt.statements, i);
        if (!codegen_generate_node(cg, stmt)) {
            return false;
        }
    }
    
    codegen_indent_decrease(cg);
    codegen_emit_line(cg, "}");
    return true;
}

// 生成表达式语句
bool statement_gen_expr(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_EXPR_STMT) return false;
    
    codegen_indent(cg);
    
    if (!codegen_expression(cg, node->expr_stmt.expression)) {
        return false;
    }
    
    codegen_emit_line(cg, ";");
    return true;
}

// 生成if语句
bool statement_gen_if(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_IF_STMT) return false;
    
    codegen_indent(cg);
    codegen_emit(cg, "if (");
    
    // 生成条件表达式
    if (!codegen_expression(cg, node->if_stmt.condition)) {
        return false;
    }
    
    codegen_emit_line(cg, ")");
    
    // 生成then分支
    if (!codegen_generate_node(cg, node->if_stmt.then_branch)) {
        return false;
    }
    
    // 生成else分支（如果存在）
    if (node->if_stmt.else_branch) {
        codegen_indent(cg);
        codegen_emit_line(cg, "else");
        
        if (!codegen_generate_node(cg, node->if_stmt.else_branch)) {
            return false;
        }
    }
    
    return true;
}

// 生成for循环
bool statement_gen_for(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_FOR_STMT) return false;
    
    // 保存循环状态
    bool old_in_loop = cg->in_loop;
    cg->in_loop = true;
    
    codegen_indent(cg);
    codegen_emit(cg, "for (");
    
    // 生成迭代变量声明
    if (node->for_stmt.pattern->type == NODE_PATTERN_IDENT) {
        const char* var_name = node->for_stmt.pattern->pattern_ident.name;
        codegen_emit(cg, "int %s = 0; ", var_name); // 简化：假设是整数索引
        
        // 这里应该生成更复杂的迭代逻辑
        // 简化版本：只生成基本的for循环结构
        codegen_emit(cg, "%s < 10; ", var_name); // 简化：固定次数
        codegen_emit(cg, "%s++", var_name);
    }
    
    codegen_emit_line(cg, ")");
    
    // 生成循环体
    if (!codegen_generate_node(cg, node->for_stmt.body)) {
        cg->in_loop = old_in_loop;
        return false;
    }
    
    // 恢复循环状态
    cg->in_loop = old_in_loop;
    return true;
}

// 生成while循环
bool statement_gen_while(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_WHILE_STMT) return false;
    
    // 保存循环状态
    bool old_in_loop = cg->in_loop;
    cg->in_loop = true;
    
    codegen_indent(cg);
    codegen_emit(cg, "while (");
    
    // 生成条件表达式
    if (!codegen_expression(cg, node->while_stmt.condition)) {
        cg->in_loop = old_in_loop;
        return false;
    }
    
    codegen_emit_line(cg, ")");
    
    // 生成循环体
    if (!codegen_generate_node(cg, node->while_stmt.body)) {
        cg->in_loop = old_in_loop;
        return false;
    }
    
    // 恢复循环状态
    cg->in_loop = old_in_loop;
    return true;
}

// 生成return语句
bool statement_gen_return(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_RETURN_STMT) return false;
    
    codegen_indent(cg);
    codegen_emit(cg, "return");
    
    if (node->return_stmt.value) {
        codegen_emit(cg, " ");
        if (!codegen_expression(cg, node->return_stmt.value)) {
            return false;
        }
    }
    
    codegen_emit_line(cg, ";");
    return true;
}

// 生成break语句
bool statement_gen_break(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_BREAK_STMT) return false;
    
    if (!cg->in_loop) {
        codegen_error(cg, "Break statement outside loop", node->line, node->column);
        return false;
    }
    
    codegen_indent(cg);
    codegen_emit_line(cg, "break;");
    return true;
}

// 生成continue语句
bool statement_gen_continue(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_CONTINUE_STMT) return false;
    
    if (!cg->in_loop) {
        codegen_error(cg, "Continue statement outside loop", node->line, node->column);
        return false;
    }
    
    codegen_indent(cg);
    codegen_emit_line(cg, "continue;");
    return true;
}