#include "expression_gen.h"
#include "../utils/utils.h"
#include <stdlib.h>

// 生成二元表达式
bool expression_gen_binary(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_BINARY_EXPR) return false;
    
    codegen_emit(cg, "(");
    
    // 生成左操作数
    if (!codegen_expression(cg, node->binary_expr.left)) {
        return false;
    }
    
    // 生成运算符
    const char* op = codegen_operator_to_c_operator(node->binary_expr.operator);
    codegen_emit(cg, " %s ", op);
    
    // 生成右操作数
    if (!codegen_expression(cg, node->binary_expr.right)) {
        return false;
    }
    
    codegen_emit(cg, ")");
    return true;
}

// 生成一元表达式
bool expression_gen_unary(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_UNARY_EXPR) return false;
    
    const char* op = codegen_operator_to_c_operator(node->unary_expr.operator);
    codegen_emit(cg, "%s", op);
    
    // 对于负号，可能需要括号
    if (node->unary_expr.operator == TOKEN_MINUS) {
        codegen_emit(cg, "(");
    }
    
    if (!codegen_expression(cg, node->unary_expr.operand)) {
        return false;
    }
    
    if (node->unary_expr.operator == TOKEN_MINUS) {
        codegen_emit(cg, ")");
    }
    
    return true;
}

// 生成字面量表达式
bool expression_gen_literal(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_LITERAL_EXPR) return false;
    
    Token token = node->literal_expr.literal_token;
    
    switch (token.type) {
        case TOKEN_INTEGER:
            codegen_emit(cg, "%.*s", token.length, token.start);
            break;
            
        case TOKEN_FLOAT:
            codegen_emit(cg, "%.*s", token.length, token.start);
            break;
            
        case TOKEN_STRING:
            codegen_emit(cg, "\"%.*s\"", token.length, token.start);
            break;
            
        case TOKEN_CHAR:
            codegen_emit(cg, "'%.*s'", token.length, token.start);
            break;
            
        case TOKEN_TRUE:
            codegen_emit(cg, "true");
            break;
            
        case TOKEN_FALSE:
            codegen_emit(cg, "false");
            break;
            
        default:
            codegen_error(cg, "Unsupported literal type", node->line, node->column);
            return false;
    }
    
    return true;
}

// 生成标识符表达式
bool expression_gen_identifier(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_IDENTIFIER_EXPR) return false;
    
    codegen_emit(cg, "%s", node->identifier_expr.name);
    return true;
}

// 生成函数调用表达式
bool expression_gen_call(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_CALL_EXPR) return false;
    
    // 生成被调用函数
    if (!codegen_expression(cg, node->call_expr.callee)) {
        return false;
    }
    
    codegen_emit(cg, "(");
    
    // 生成参数
    for (size_t i = 0; i < vector_size(node->call_expr.arguments); i++) {
        Node* arg = vector_get(node->call_expr.arguments, i);
        if (!codegen_expression(cg, arg)) {
            return false;
        }
        
        if (i < vector_size(node->call_expr.arguments) - 1) {
            codegen_emit(cg, ", ");
        }
    }
    
    codegen_emit(cg, ")");
    return true;
}

// 生成数组访问表达式
bool expression_gen_array_access(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_ARRAY_ACCESS_EXPR) return false;
    
    // 生成数组表达式
    if (!codegen_expression(cg, node->array_access_expr.array)) {
        return false;
    }
    
    codegen_emit(cg, "[");
    
    // 生成索引表达式
    if (!codegen_expression(cg, node->array_access_expr.index)) {
        return false;
    }
    
    codegen_emit(cg, "]");
    return true;
}

// 生成字段访问表达式
bool expression_gen_field_access(CodeGenerator* cg, Node* node) {
    if (!cg || !node || node->type != NODE_FIELD_ACCESS_EXPR) return false;
    
    // 生成对象表达式
    if (!codegen_expression(cg, node->field_access_expr.object)) {
        return false;
    }
    
    codegen_emit(cg, ".%s", node->field_access_expr.field_name);
    return true;
}