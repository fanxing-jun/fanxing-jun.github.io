#include "type_checker.h"
#include "type_inference.h"
#include "type_compatibility.h"
#include "../utils/utils.h"
#include "../utils/string_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建类型检查器
TypeChecker* type_checker_create(SymbolTable* symbol_table) {
    TypeChecker* tc = malloc(sizeof(TypeChecker));
    if (!tc) return NULL;
    
    tc->symbol_table = symbol_table;
    tc->errors = vector_create(free_direct);
    tc->warnings = vector_create(free_direct);
    tc->in_loop = false;
    tc->in_function = false;
    tc->current_return_type = NULL;
    tc->current_function = NULL;
    
    return tc;
}

// 释放类型检查器
void type_checker_free(TypeChecker* tc) {
    if (!tc) return;
    
    vector_free(tc->errors);
    vector_free(tc->warnings);
    free(tc);
}

// 类型检查程序
bool type_check_program(TypeChecker* tc, Node* program) {
    if (!tc || !program || program->type != NODE_PROGRAM) {
        return false;
    }
    
    bool success = true;
    
    // 检查所有声明
    for (size_t i = 0; i < vector_size(program->program.declarations); i++) {
        Node* decl = vector_get(program->program.declarations, i);
        if (!type_check_node(tc, decl)) {
            success = false;
        }
    }
    
    return success && !type_checker_has_errors(tc);
}

// 类型检查单个节点
bool type_check_node(TypeChecker* tc, Node* node) {
    if (!tc || !node) return false;
    
    // 设置节点类型（用于后续阶段）
    if (!node->node_type) {
        node->node_type = infer_expression_type(tc, node);
    }
    
    switch (node->type) {
        // 声明
        case NODE_PROGRAM:
            return type_check_program(tc, node);
        case NODE_FUNCTION_DECL:
            return type_check_function_decl(tc, node);
        case NODE_VAR_DECL:
            return type_check_variable_decl(tc, node);
        case NODE_STRUCT_DECL:
            return type_check_struct_decl(tc, node);
        case NODE_ENUM_DECL:
            return type_check_enum_decl(tc, node);
            
        // 语句
        case NODE_BLOCK_STMT:
            return type_check_block_stmt(tc, node);
        case NODE_EXPR_STMT:
            return type_check_expr_stmt(tc, node);
        case NODE_IF_STMT:
            return type_check_if_stmt(tc, node);
        case NODE_FOR_STMT:
            return type_check_for_stmt(tc, node);
        case NODE_WHILE_STMT:
            return type_check_while_stmt(tc, node);
        case NODE_MATCH_STMT:
            return type_check_match_stmt(tc, node);
        case NODE_RETURN_STMT:
            return type_check_return_stmt(tc, node);
        case NODE_BREAK_STMT:
            return type_check_break_stmt(tc, node);
        case NODE_CONTINUE_STMT:
            return type_check_continue_stmt(tc, node);
            
        // 表达式 - 这些函数返回类型而不是bool
        case NODE_BINARY_EXPR:
        case NODE_UNARY_EXPR:
        case NODE_CALL_EXPR:
        case NODE_METHOD_CALL_EXPR:
        case NODE_FIELD_ACCESS_EXPR:
        case NODE_ARRAY_ACCESS_EXPR:
        case NODE_LITERAL_EXPR:
        case NODE_IDENTIFIER_EXPR:
        case NODE_GROUP_EXPR:
        case NODE_IF_EXPR:
        case NODE_MATCH_EXPR:
        case NODE_ARRAY_EXPR:
        case NODE_STRUCT_EXPR:
        case NODE_CLOSURE_EXPR:
        case NODE_AWAIT_EXPR:
            {
                Type* result_type = type_check_expression(tc, node);
                return result_type != NULL;
            }
            
        default:
            type_error(tc, "Unsupported node type in type checking", 
                      node->line, node->column);
            return false;
    }
}

// ===== 声明类型检查 =====

// 检查函数声明
bool type_check_function_decl(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_FUNCTION_DECL) return false;
    
    // 保存当前状态
    bool old_in_function = tc->in_function;
    Type* old_return_type = tc->current_return_type;
    Node* old_current_function = tc->current_function;
    
    // 设置新状态
    tc->in_function = true;
    tc->current_function = node;
    
    // 解析返回类型
    Type* return_type = type_create_basic(TYPE_VOID, "void");
    if (node->func_decl.return_type) {
        return_type = resolve_type_annotation(tc, node->func_decl.return_type);
    }
    tc->current_return_type = return_type;
    
    // 进入函数作用域
    symbol_table_enter_scope(tc->symbol_table, SCOPE_FUNCTION);
    
    // 检查参数
    bool success = true;
    for (size_t i = 0; i < vector_size(node->func_decl.params); i++) {
        Node* param = vector_get(node->func_decl.params, i);
        if (!type_check_node(tc, param)) {
            success = false;
        }
    }
    
    // 检查函数体
    if (node->func_decl.body) {
        if (!type_check_node(tc, node->func_decl.body)) {
            success = false;
        }
    }
    
    // 退出函数作用域
    symbol_table_exit_scope(tc->symbol_table);
    
    // 恢复状态
    tc->in_function = old_in_function;
    tc->current_return_type = old_return_type;
    tc->current_function = old_current_function;
    
    return success;
}

// 检查变量声明
bool type_check_variable_decl(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_VAR_DECL) return false;
    
    Type* declared_type = NULL;
    Type* value_type = NULL;
    
    // 解析声明的类型
    if (node->var_decl.type_annotation) {
        declared_type = resolve_type_annotation(tc, node->var_decl.type_annotation);
    }
    
    // 检查初始值表达式
    if (node->var_decl.value) {
        value_type = type_check_expression(tc, node->var_decl.value);
        if (!value_type) {
            return false;
        }
    }
    
    // 类型推断或兼容性检查
    Type* variable_type = declared_type;
    if (!declared_type && value_type) {
        // 类型推断：使用值的类型
        variable_type = value_type;
    } else if (declared_type && value_type) {
        // 检查类型兼容性
        if (!types_are_assignable(tc, declared_type, value_type)) {
            type_error_with_types(tc, "Type mismatch in variable declaration",
                                value_type, declared_type, node->line, node->column);
            return false;
        }
    } else if (!declared_type && !value_type) {
        type_error(tc, "Variable declaration must have either type annotation or initial value",
                  node->line, node->column);
        return false;
    }
    
    // 创建符号并定义
    Symbol* symbol = symbol_create_variable(
        node->var_decl.name, node, variable_type,
        node->var_decl.is_mutable, false
    );
    
    if (!symbol_table_define(tc->symbol_table, symbol)) {
        symbol_free(symbol);
        return false;
    }
    
    return true;
}

// 检查结构体声明
bool type_check_struct_decl(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_STRUCT_DECL) return false;
    
    // 创建结构体符号
    Symbol* struct_symbol = symbol_create_struct(node->struct_decl.name, node, NULL);
    if (!struct_symbol) return false;
    
    // 进入结构体作用域
    symbol_table_enter_scope(tc->symbol_table, SCOPE_STRUCT);
    
    // 检查字段
    bool success = true;
    for (size_t i = 0; i < vector_size(node->struct_decl.fields); i++) {
        Node* field = vector_get(node->struct_decl.fields, i);
        if (field->type != NODE_FIELD_DEF) continue;
        
        Type* field_type = resolve_type_annotation(tc, field->field_def.type_annotation);
        if (!field_type) {
            success = false;
            continue;
        }
        
        // 创建字段符号
        Symbol* field_symbol = symbol_create_variable(
            field->field_def.name, field, field_type, false, false
        );
        field_symbol->visibility = field->field_def.is_public ? 
            VISIBILITY_PUBLIC : VISIBILITY_PRIVATE;
        
        if (!symbol_table_define(tc->symbol_table, field_symbol)) {
            symbol_free(field_symbol);
            success = false;
        } else {
            // 添加到结构体的字段列表
            vector_push(struct_symbol->structure.fields, field_symbol);
        }
    }
    
    // 退出结构体作用域
    symbol_table_exit_scope(tc->symbol_table);
    
    // 定义结构体符号
    if (success && !symbol_table_define(tc->symbol_table, struct_symbol)) {
        symbol_free(struct_symbol);
        return false;
    }
    
    return success;
}

// ===== 语句类型检查 =====

// 检查块语句
bool type_check_block_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_BLOCK_STMT) return false;
    
    symbol_table_enter_scope(tc->symbol_table, SCOPE_BLOCK);
    
    bool success = true;
    for (size_t i = 0; i < vector_size(node->block_stmt.statements); i++) {
        Node* stmt = vector_get(node->block_stmt.statements, i);
        if (!type_check_node(tc, stmt)) {
            success = false;
        }
    }
    
    symbol_table_exit_scope(tc->symbol_table);
    return success;
}

// 检查表达式语句
bool type_check_expr_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_EXPR_STMT) return false;
    
    Type* expr_type = type_check_expression(tc, node->expr_stmt.expression);
    return expr_type != NULL;
}

// 检查if语句
bool type_check_if_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_IF_STMT) return false;
    
    // 检查条件表达式
    Type* cond_type = type_check_expression(tc, node->if_stmt.condition);
    if (!cond_type) return false;
    
    if (!is_boolean_type(cond_type)) {
        type_error_with_types(tc, "If condition must be boolean", 
                            cond_type, type_create_basic(TYPE_BOOL, "bool"),
                            node->line, node->column);
        return false;
    }
    
    bool success = true;
    
    // 检查then分支
    if (!type_check_node(tc, node->if_stmt.then_branch)) {
        success = false;
    }
    
    // 检查else分支（如果存在）
    if (node->if_stmt.else_branch) {
        if (!type_check_node(tc, node->if_stmt.else_branch)) {
            success = false;
        }
    }
    
    return success;
}

// 检查for循环
bool type_check_for_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_FOR_STMT) return false;
    
    // 检查可迭代对象
    Type* iterable_type = type_check_expression(tc, node->for_stmt.iterable);
    if (!iterable_type) return false;
    
    // 简化：假设所有可迭代对象都是数组或切片
    Type* element_type = NULL;
    if (iterable_type->kind == TYPE_ARRAY) {
        element_type = iterable_type->array.element_type;
    } else if (iterable_type->kind == TYPE_SLICE) {
        element_type = iterable_type->slice.element_type;
    } else {
        type_error(tc, "For loop requires an iterable type (array or slice)",
                  node->line, node->column);
        return false;
    }
    
    // 进入循环作用域
    bool old_in_loop = tc->in_loop;
    tc->in_loop = true;
    symbol_table_enter_scope(tc->symbol_table, SCOPE_LOOP);
    
    // 检查模式绑定
    if (!type_check_pattern(tc, node->for_stmt.pattern, element_type)) {
        tc->in_loop = old_in_loop;
        symbol_table_exit_scope(tc->symbol_table);
        return false;
    }
    
    // 检查循环体
    bool success = type_check_node(tc, node->for_stmt.body);
    
    // 退出循环作用域
    symbol_table_exit_scope(tc->symbol_table);
    tc->in_loop = old_in_loop;
    
    return success;
}

// 检查return语句
bool type_check_return_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_RETURN_STMT) return false;
    
    if (!tc->in_function) {
        type_error(tc, "Return statement outside function", 
                  node->line, node->column);
        return false;
    }
    
    Type* return_value_type = NULL;
    if (node->return_stmt.value) {
        return_value_type = type_check_expression(tc, node->return_stmt.value);
        if (!return_value_type) return false;
    } else {
        // 没有返回值的return，对应void
        return_value_type = type_create_basic(TYPE_VOID, "void");
    }
    
    // 检查返回类型兼容性
    if (!types_are_compatible(tc, return_value_type, tc->current_return_type)) {
        type_error_with_types(tc, "Return type mismatch",
                            return_value_type, tc->current_return_type,
                            node->line, node->column);
        return false;
    }
    
    return true;
}

// 检查break语句
bool type_check_break_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_BREAK_STMT) return false;
    
    if (!tc->in_loop) {
        type_error(tc, "Break statement outside loop", 
                  node->line, node->column);
        return false;
    }
    
    return true;
}

// 检查continue语句
bool type_check_continue_stmt(TypeChecker* tc, Node* node) {
    if (!tc || node->type != NODE_CONTINUE_STMT) return false;
    
    if (!tc->in_loop) {
        type_error(tc, "Continue statement outside loop", 
                  node->line, node->column);
        return false;
    }
    
    return true;
}