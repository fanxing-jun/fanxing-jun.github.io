#include "type_compatibility.h"
#include "../utils/utils.h"
#include <string.h>

// 检查类型是否相等
bool types_are_equal(Type* a, Type* b) {
    if (a == b) return true;
    if (!a || !b) return false;
    if (a->kind != b->kind) return false;
    
    switch (a->kind) {
        case TYPE_VOID:
        case TYPE_INT8:
        case TYPE_INT16:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_UINT8:
        case TYPE_UINT16:
        case TYPE_UINT32:
        case TYPE_UINT64:
        case TYPE_FLOAT32:
        case TYPE_FLOAT64:
        case TYPE_BOOL:
        case TYPE_CHAR:
        case TYPE_STRING:
            return true;
            
        case TYPE_ARRAY:
            return types_are_equal(a->array.element_type, b->array.element_type) &&
                   a->array.array_size == b->array.array_size;
            
        case TYPE_SLICE:
            return types_are_equal(a->slice.element_type, b->slice.element_type);
            
        case TYPE_TUPLE:
            if (vector_size(a->tuple.element_types) != vector_size(b->tuple.element_types)) {
                return false;
            }
            for (size_t i = 0; i < vector_size(a->tuple.element_types); i++) {
                Type* a_elem = vector_get(a->tuple.element_types, i);
                Type* b_elem = vector_get(b->tuple.element_types, i);
                if (!types_are_equal(a_elem, b_elem)) {
                    return false;
                }
            }
            return true;
            
        case TYPE_FUNCTION:
            if (!types_are_equal(a->function.return_type, b->function.return_type)) {
                return false;
            }
            if (vector_size(a->function.param_types) != vector_size(b->function.param_types)) {
                return false;
            }
            for (size_t i = 0; i < vector_size(a->function.param_types); i++) {
                Type* a_param = vector_get(a->function.param_types, i);
                Type* b_param = vector_get(b->function.param_types, i);
                if (!types_are_equal(a_param, b_param)) {
                    return false;
                }
            }
            return true;
            
        default:
            return false;
    }
}

// 检查类型兼容性（宽松检查）
bool types_are_compatible(TypeChecker* tc, Type* actual, Type* expected) {
    if (!actual || !expected) return false;
    
    // 未知类型与任何类型兼容（在推断阶段）
    if (actual->kind == TYPE_UNKNOWN || expected->kind == TYPE_UNKNOWN) {
        return true;
    }
    
    // 相同类型总是兼容
    if (types_are_equal(actual, expected)) {
        return true;
    }
    
    // 数值类型兼容性
    if (is_numeric_type(actual) && is_numeric_type(expected)) {
        return true;
    }
    
    // 数组到切片的兼容性
    if (actual->kind == TYPE_ARRAY && expected->kind == TYPE_SLICE) {
        return types_are_equal(actual->array.element_type, expected->slice.element_type);
    }
    
    // void兼容性：void只能与void兼容
    if (is_void_type(actual) || is_void_type(expected)) {
        return is_void_type(actual) && is_void_type(expected);
    }
    
    return false;
}

// 检查赋值兼容性（严格检查）
bool types_are_assignable(TypeChecker* tc, Type* target, Type* source) {
    if (!target || !source) return false;
    
    // 目标类型必须明确
    if (target->kind == TYPE_UNKNOWN) {
        return false;
    }
    
    // 基本规则：类型必须相等或兼容
    if (!types_are_compatible(tc, source, target)) {
        return false;
    }
    
    // 额外规则：不能将float赋值给int（即使数值上可能）
    if (is_integer_type(target) && is_float_type(source)) {
        type_warning(tc, "Possible loss of precision assigning float to integer",
                    -1, -1); // 行号列号在调用处提供
        return false;
    }
    
    return true;
}

// 类型特征检查
bool is_numeric_type(Type* type) {
    if (!type) return false;
    return is_integer_type(type) || is_float_type(type);
}

bool is_integer_type(Type* type) {
    if (!type) return false;
    return type->kind >= TYPE_INT8 && type->kind <= TYPE_UINT64;
}

bool is_float_type(Type* type) {
    if (!type) return false;
    return type->kind == TYPE_FLOAT32 || type->kind == TYPE_FLOAT64;
}

bool is_boolean_type(Type* type) {
    if (!type) return false;
    return type->kind == TYPE_BOOL;
}

bool is_void_type(Type* type) {
    if (!type) return false;
    return type->kind == TYPE_VOID;
}

// 解析类型注解
Type* resolve_type_annotation(TypeChecker* tc, Node* type_annotation) {
    if (!tc || !type_annotation) return NULL;
    
    if (type_annotation->type == NODE_TYPE_ANNOTATION) {
        return type_annotation->type_annotation.type;
    }
    
    // 处理标识符类型（如 "int32", "string" 等）
    if (type_annotation->type == NODE_IDENTIFIER_EXPR) {
        Symbol* type_symbol = symbol_table_resolve_type(tc->symbol_table, 
                                                       type_annotation->identifier_expr.name);
        if (type_symbol && type_symbol->symbol_type) {
            return type_copy(type_symbol->symbol_type);
        }
    }
    
    return NULL;
}