#include "type_inference.h"
#include "type_compatibility.h"
#include "../utils/utils.h"
#include <stdlib.h>

// 推断表达式类型
Type* infer_expression_type(TypeChecker* tc, Node* node) {
    if (!tc || !node) return NULL;
    
    // 如果节点已经有类型，直接返回
    if (node->node_type) {
        return node->node_type;
    }
    
    switch (node->type) {
        case NODE_LITERAL_EXPR:
            return infer_literal_type(tc, node);
            
        case NODE_IDENTIFIER_EXPR:
            return infer_identifier_type(tc, node);
            
        case NODE_BINARY_EXPR:
            {
                Type* left_type = infer_expression_type(tc, node->binary_expr.left);
                Type* right_type = infer_expression_type(tc, node->binary_expr.right);
                
                if (!left_type || !right_type) return NULL;
                
                return infer_binary_expression_type(tc, node->binary_expr.operator, 
                                                  left_type, right_type);
            }
            
        case NODE_UNARY_EXPR:
            {
                Type* operand_type = infer_expression_type(tc, node->unary_expr.operand);
                if (!operand_type) return NULL;
                
                return infer_unary_expression_type(tc, node->unary_expr.operator, operand_type);
            }
            
        case NODE_CALL_EXPR:
            return infer_call_expression_type(tc, node);
            
        case NODE_IF_EXPR:
            {
                Type* cond_type = infer_expression_type(tc, node->if_expr.condition);
                Type* then_type = infer_expression_type(tc, node->if_expr.then_branch);
                Type* else_type = infer_expression_type(tc, node->if_expr.else_branch);
                
                if (!cond_type || !then_type || !else_type) return NULL;
                
                // if表达式的类型是两个分支的共同类型
                return get_common_type(tc, then_type, else_type);
            }
            
        case NODE_ARRAY_EXPR:
            {
                if (vector_is_empty(node->array_expr.elements)) {
                    // 空数组，需要类型注解或上下文信息
                    return type_create_array(type_create_basic(TYPE_UNKNOWN, "unknown"), 0);
                }
                
                // 推断第一个元素的类型
                Node* first_element = vector_get(node->array_expr.elements, 0);
                Type* element_type = infer_expression_type(tc, first_element);
                if (!element_type) return NULL;
                
                // 检查所有元素类型是否一致
                for (size_t i = 1; i < vector_size(node->array_expr.elements); i++) {
                    Node* element = vector_get(node->array_expr.elements, i);
                    Type* current_type = infer_expression_type(tc, element);
                    
                    if (!types_are_equal(element_type, current_type)) {
                        type_error_with_types(tc, "Array elements must have the same type",
                                            current_type, element_type,
                                            node->line, node->column);
                        return NULL;
                    }
                }
                
                return type_create_array(element_type, vector_size(node->array_expr.elements));
            }
            
        default:
            // 对于其他表达式，返回未知类型，等待具体检查
            return type_create_basic(TYPE_UNKNOWN, "unknown");
    }
}

// 推断字面量类型
Type* infer_literal_type(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_LITERAL_EXPR) return NULL;
    
    Token token = node->literal_expr.literal_token;
    
    switch (token.type) {
        case TOKEN_INTEGER:
            return type_create_basic(TYPE_INT32, "int32");
            
        case TOKEN_FLOAT:
            return type_create_basic(TYPE_FLOAT64, "float64");
            
        case TOKEN_STRING:
            return type_create_basic(TYPE_STRING, "string");
            
        case TOKEN_CHAR:
            return type_create_basic(TYPE_CHAR, "char");
            
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            return type_create_basic(TYPE_BOOL, "bool");
            
        default:
            return type_create_basic(TYPE_UNKNOWN, "unknown");
    }
}

// 推断标识符类型
Type* infer_identifier_type(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_IDENTIFIER_EXPR) return NULL;
    
    Symbol* symbol = symbol_table_resolve(tc->symbol_table, node->identifier_expr.name);
    if (!symbol) {
        type_error(tc, "Undefined identifier", node->line, node->column);
        return NULL;
    }
    
    return symbol->symbol_type;
}

// 推断二元表达式类型
Type* infer_binary_expression_type(TypeChecker* tc, TokenType operator, Type* left, Type* right) {
    if (!left || !right) return NULL;
    
    switch (operator) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_STAR:
        case TOKEN_SLASH:
            // 算术运算：需要数值类型
            if (is_numeric_type(left) && is_numeric_type(right)) {
                return get_common_type(tc, left, right);
            }
            break;
            
        case TOKEN_PERCENT:
            // 取模运算：需要整数类型
            if (is_integer_type(left) && is_integer_type(right)) {
                return get_common_type(tc, left, right);
            }
            break;
            
        case TOKEN_EQEQ:
        case TOKEN_NEQ:
        case TOKEN_LT:
        case TOKEN_GT:
        case TOKEN_LTEQ:
        case TOKEN_GTEQ:
            // 比较运算：返回布尔类型
            if (types_are_compatible(tc, left, right)) {
                return type_create_basic(TYPE_BOOL, "bool");
            }
            break;
            
        case TOKEN_AND:
        case TOKEN_OR:
            // 逻辑运算：需要布尔类型
            if (is_boolean_type(left) && is_boolean_type(right)) {
                return type_create_basic(TYPE_BOOL, "bool");
            }
            break;
            
        default:
            break;
    }
    
    return NULL;
}

// 推断一元表达式类型
Type* infer_unary_expression_type(TypeChecker* tc, TokenType operator, Type* operand) {
    if (!operand) return NULL;
    
    switch (operator) {
        case TOKEN_NOT:
            if (is_boolean_type(operand)) {
                return type_create_basic(TYPE_BOOL, "bool");
            }
            break;
            
        case TOKEN_MINUS:
            if (is_numeric_type(operand)) {
                return operand; // 保持相同类型
            }
            break;
            
        case TOKEN_PLUS:
            if (is_numeric_type(operand)) {
                return operand; // 保持相同类型
            }
            break;
            
        default:
            break;
    }
    
    return NULL;
}

// 推断函数调用类型
Type* infer_call_expression_type(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_CALL_EXPR) return NULL;
    
    // 推断被调用函数的类型
    Type* callee_type = infer_expression_type(tc, node->call_expr.callee);
    if (!callee_type || callee_type->kind != TYPE_FUNCTION) {
        return NULL;
    }
    
    // 返回函数的返回类型
    return callee_type->function.return_type;
}

// 获取共同类型（类型提升）
Type* get_common_type(TypeChecker* tc, Type* a, Type* b) {
    if (!a || !b) return NULL;
    
    if (types_are_equal(a, b)) {
        return a;
    }
    
    // 数值类型提升规则
    if (is_numeric_type(a) && is_numeric_type(b)) {
        // 简化规则：总是提升到float64
        if (a->kind == TYPE_FLOAT64 || b->kind == TYPE_FLOAT64) {
            return type_create_basic(TYPE_FLOAT64, "float64");
        }
        if (a->kind == TYPE_FLOAT32 || b->kind == TYPE_FLOAT32) {
            return type_create_basic(TYPE_FLOAT32, "float32");
        }
        // 整数提升到int32
        return type_create_basic(TYPE_INT32, "int32");
    }
    
    // 如果类型不兼容，返回第一个类型（实际应该报错）
    return a;
}