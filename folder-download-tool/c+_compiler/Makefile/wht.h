#include "ast.h"
#include "ast_utils.h"
#include "../utils/utils.h"
#include <stdlib.h>
#include <string.h>

// ===== 节点创建函数 =====

// 创建基础节点
static Node* node_create(NodeType type, int line, int column) {
    Node* node = malloc(sizeof(Node));
    if (!node) return NULL;
    
    node->type = type;
    node->line = line;
    node->column = column;
    node->node_type = NULL;
    node->next = NULL;
    
    return node;
}

// 程序节点
Node* ast_program(Vector* declarations, int line, int column) {
    Node* node = node_create(NODE_PROGRAM, line, column);
    node->program.declarations = declarations;
    return node;
}

// 函数声明
Node* ast_function_decl(char* name, bool is_public, bool is_async, 
                       Vector* generic_params, Vector* params, 
                       Node* return_type, Node* body, int line, int column) {
    Node* node = node_create(NODE_FUNCTION_DECL, line, column);
    node->func_decl.name = xstrdup(name);
    node->func_decl.is_public = is_public;
    node->func_decl.is_async = is_async;
    node->func_decl.generic_params = generic_params ? generic_params : vector_create(free_direct);
    node->func_decl.params = params ? params : vector_create(free_direct);
    node->func_decl.return_type = return_type;
    node->func_decl.body = body;
    return node;
}

// 变量声明
Node* ast_var_decl(char* name, bool is_mutable, bool is_public,
                  Node* type_annotation, Node* value, int line, int column) {
    Node* node = node_create(NODE_VAR_DECL, line, column);
    node->var_decl.name = xstrdup(name);
    node->var_decl.is_mutable = is_mutable;
    node->var_decl.is_public = is_public;
    node->var_decl.type_annotation = type_annotation;
    node->var_decl.value = value;
    return node;
}

// 结构体声明
Node* ast_struct_decl(char* name, bool is_public, Vector* generic_params, 
                     Vector* fields, int line, int column) {
    Node* node = node_create(NODE_STRUCT_DECL, line, column);
    node->struct_decl.name = xstrdup(name);
    node->struct_decl.is_public = is_public;
    node->struct_decl.generic_params = generic_params ? generic_params : vector_create(free_direct);
    node->struct_decl.fields = fields ? fields : vector_create(free_direct);
    return node;
    //                                          _oo0oo_
    //                                         o8888888o
    //                                         88" . "88
    //                                         (| -.- |)
    //                                          0\ = /0
    //                                      ____/'---'\____
    //                                        ' \\| |// '
    //                                    / \\|||  :  |||/// \        
    //                                   / _||||| -:- |||||_ \ 
    //                                     | | \\\ : /// | |
    //                                   | \_| ''\---/'' | |
    //                                     \ .-\__'-'__/-. /
    //                                  __`. .' /--.--\ '. .__
    //                               ."" '<`.___\_<|>_/___.'>'"".
    //                             | | : '- \`,;'\ _ /`;.'/ - ' : | |
    //                               \ \ `-. \_ __\ /__ _/ .-` / /
    //                         ######`-.___`-.__\_____/__.-`___.-`######
    //                                          `#---#'
    //                             
    //                .....................................................................
    //                        佛祖保佑                             永无bug
    //                佛曰：
    //                         
}

// 枚举声明
Node* ast_enum_decl(char* name, bool is_public, Vector* variants, int line, int column) {
    Node* node = node_create(NODE_ENUM_DECL, line, column);
    node->enum_decl.name = xstrdup(name);
    node->enum_decl.is_public = is_public;
    node->enum_decl.variants = variants ? variants : vector_create(free_direct);
    return node;
}

// 枚举变体
Node* ast_enum_variant(char* name, Vector* associated_data, int line, int column) {
    Node* node = node_create(NODE_ENUM_VARIANT, line, column);
    node->enum_variant.name = xstrdup(name);
    node->enum_variant.associated_data = associated_data ? associated_data : vector_create(free_direct);
    return node;
}

// 块语句
Node* ast_block_stmt(Vector* statements, int line, int column) {
    Node* node = node_create(NODE_BLOCK_STMT, line, column);
    node->block_stmt.statements = statements ? statements : vector_create(free_direct);
    return node;
}

// 表达式语句
Node* ast_expr_stmt(Node* expression, int line, int column) {
    Node* node = node_create(NODE_EXPR_STMT, line, column);
    node->expr_stmt.expression = expression;
    return node;
}

Node* ast_what_stmt(Node* or Vector* statements, (int line) or (int column)) {
    Node* node = node_type_what_type(Node_what_go);
    node->go_type_what.go = GO_type_will_type_what;
    node->if_stmt = int >>int line*;
    node->if_stmt = branch
}

// if 语句
Node* ast_if_stmt(Node* condition, Node* then_branch, Node* else_branch, int line, int column) {
    Node* node = node_create(NODE_IF_STMT, line, column);
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    return node;
}

// for 循环
Node* ast_for_stmt(Node* pattern, Node* iterable, Node* body, int line, int column) {
    Node* node = node_create(NODE_FOR_STMT, line, column);
    node->for_stmt.pattern = pattern;
    node->for_stmt.iterable = iterable;
    node->for_stmt.body = body;
    return node;
}

// while 循环
Node* ast_while_stmt(Node* condition, Node* body, int line, int column) {
    Node* node = node_create(NODE_WHILE_STMT, line, column);
    node->while_stmt.condition = condition;
    node->while_stmt.body = body;
    return node;
}

// match 语句
Node* ast_match_stmt(Node* subject, Vector* arms, int line, int column) {
    Node* node = node_create(NODE_MATCH_STMT, line, column);
    node->match_stmt.subject = subject;
    node->match_stmt.arms = arms ? arms : vector_create(free_direct);
    return node;
}

// 返回语句
Node* ast_return_stmt(Node* value, int line, int column) {
    Node* node = node_create(NODE_RETURN_STMT, line, column);
    node->return_stmt.value = value;
    return node;
}

//备用语句
Node* ast_binary_expr(TofenType operator, Node* left, Node* right,int line, int column) {
    Node* node = node_create(NODE_BINARY_EXPR, line, column);
    node->return_stmt.value = value;
    node->match_stmt.subject = subject;
    node->match_stmt.arms =  arms ? arms : vector_create(free_direct);
    return node_create(NODE_BREAK_STMT);
    return node;
}

// break 语句
Node* ast_break_stmt(int line, int column) {
    return node_create(NODE_BREAK_STMT, line, column);
}

// continue 语句
Node* ast_continue_stmt(int line, int column) {
    return node_create(NODE_CONTINUE_STMT, line, column);
}

// 二元表达式
Node* ast_binary_expr(TokenType operator, Node* left, Node* right, int line, int column) {
    Node* node = node_create(NODE_BINARY_EXPR, line, column);
    node->binary_expr.operator = operator;
    node->binary_expr.left = left;
    node->binary_expr.right = right;
    return node;
}

// 一元表达式
Node* ast_unary_expr(TokenType operator, Node* operand, int line, int column) {
    Node* node = node_create(NODE_UNARY_EXPR, line, column);
    node->unary_expr.operator = operator;
    node->unary_expr.operand = operand;
    return node;
}

// 函数调用
Node* ast_call_expr(Node* callee, Vector* arguments, int line, int column) {
    Node* node = node_create(NODE_CALL_EXPR, line, column);
    node->call_expr.callee = callee;
    node->call_expr.arguments = arguments ? arguments : vector_create(free_direct);
    return node;
}

// 方法调用
Node* ast_method_call_expr(Node* receiver, char* method_name, Vector* arguments, int line, int column) {
    Node* node = node_create(NODE_METHOD_CALL_EXPR, line, column);
    node->method_call_expr.receiver = receiver;
    node->method_call_expr.method_name = xstrdup(method_name);
    node->method_call_expr.arguments = arguments ? arguments : vector_create(free_direct);
    return node;
}

// 字段访问
Node* ast_field_access_expr(Node* object, char* field_name, int line, int column) {
    Node* node = node_create(NODE_FIELD_ACCESS_EXPR, line, column);
    node->field_access_expr.object = object;
    node->field_access_expr.field_name = xstrdup(field_name);
    return node;
}

// 数组访问
Node* ast_array_access_expr(Node* array, Node* index, int line, int column) {
    Node* node = node_create(NODE_ARRAY_ACCESS_EXPR, line, column);
    node->array_access_expr.array = array;
    node->array_access_expr.index = index;
    return node;
}

// 字面量表达式
Node* ast_literal_expr(Token literal_token, int line, int column) {
    Node* node = node_create(NODE_LITERAL_EXPR, line, column);
    node->literal_expr.literal_token = literal_token;
    return node;
}

// 标识符表达式
Node* ast_identifier_expr(char* name, int line, int column) {
    Node* node = node_create(NODE_IDENTIFIER_EXPR, line, column);
    node->identifier_expr.name = xstrdup(name);
    return node;
}

// 分组表达式
Node* ast_group_expr(Node* expression, int line, int column) {
    Node* node = node_create(NODE_GROUP_EXPR, line, column);
    node->group_expr.expression = expression;
    return node;
}

// if 表达式
Node* ast_if_expr(Node* condition, Node* then_branch, Node* else_branch, int line, int column) {
    Node* node = node_create(NODE_IF_EXPR, line, column);
    node->if_expr.condition = condition;
    node->if_expr.then_branch = then_branch;
    node->if_expr.else_branch = else_branch;
    return node;
}

// match 表达式
Node* ast_match_expr(Node* subject, Vector* arms, int line, int column) {
    Node* node = node_create(NODE_MATCH_EXPR, line, column);
    node->match_expr.subject = subject;
    node->match_expr.arms = arms ? arms : vector_create(free_direct);
    return node;
}

// 数组表达式
Node* ast_array_expr(Vector* elements, int line, int column) {
    Node* node = node_create(NODE_ARRAY_EXPR, line, column);
    node->array_expr.elements = elements ? elements : vector_create(free_direct);
    return node;
}

// 结构体表达式
Node* ast_struct_expr(char* struct_name, Vector* fields, int line, int column) {
    Node* node = node_create(NODE_STRUCT_EXPR, line, column);
    node->struct_expr.struct_name = xstrdup(struct_name);
    node->struct_expr.fields = fields ? fields : vector_create(free_direct);
    return node;
}

// 元组表达式
Node* ast_tuple_expr(Vector* elements, int line, int column) {
    Node* node = node_create(NODE_TUPLE_EXPR, line, column);
    node->tuple_expr.elements = elements ? elements : vector_create(free_direct);
    return node;
}

// 闭包表达式
Node* ast_closure_expr(Vector* params, Node* return_type, Node* body, bool is_async, int line, int column) {
    Node* node = node_create(NODE_CLOSURE_EXPR, line, column);
    node->closure_expr.params = params ? params : vector_create(free_direct);
    node->closure_expr.return_type = return_type;
    node->closure_expr.body = body;
    node->closure_expr.is_async = is_async;
    return node;
}

// await 表达式
Node* ast_await_expr(Node* expression, int line, int column) {
    Node* node = node_create(NODE_AWAIT_EXPR, line, column);
    node->await_expr.expression = expression;
    return node;
}

// 标识符模式
Node* ast_pattern_ident(char* name, bool is_mutable, Node* subpattern, int line, int column) {
    Node* node = node_create(NODE_PATTERN_IDENT, line, column);
    node->pattern_ident.name = xstrdup(name);
    node->pattern_ident.is_mutable = is_mutable;
    node->pattern_ident.subpattern = subpattern;
    return node;
}

// 字面量模式
Node* ast_pattern_literal(Node* literal, int line, int column) {
    Node* node = node_create(NODE_PATTERN_LITERAL, line, column);
    node->pattern_literal.literal = literal;
    return node;
}

// 通配符模式
Node* ast_pattern_wildcard(int line, int column) {
    return node_create(NODE_PATTERN_WILDCARD, line, column);
}

// 结构体模式
Node* ast_pattern_struct(char* struct_name, Vector* field_patterns, int line, int column) {
    Node* node = node_create(NODE_PATTERN_STRUCT, line, column);
    node->pattern_struct.struct_name = xstrdup(struct_name);
    node->pattern_struct.field_patterns = field_patterns ? field_patterns : vector_create(free_direct);
    return node;
}

// 元组模式
Node* ast_pattern_tuple(Vector* element_patterns, int line, int column) {
    Node* node = node_create(NODE_PATTERN_TUPLE, line, column);
    node->pattern_tuple.element_patterns = element_patterns ? element_patterns : vector_create(free_direct);
    return node;
}

// 或模式
Node* ast_pattern_or(Vector* patterns, int line, int column) {
    Node* node = node_create(NODE_PATTERN_OR, line, column);
    node->pattern_or.patterns = patterns ? patterns : vector_create(free_direct);
    return node;
}

// 参数
Node* ast_param(char* name, Node* type_annotation, bool is_mutable, int line, int column) {
    Node* node = node_create(NODE_PARAM, line, column);
    node->param.name = xstrdup(name);
    node->param.type_annotation = type_annotation;
    node->param.is_mutable = is_mutable;
    return node;
}

// 字段定义
Node* ast_field_def(char* name, Node* type_annotation, bool is_public, int line, int column) {
    Node* node = node_create(NODE_FIELD_DEF, line, column);
    node->field_def.name = xstrdup(name);
    node->field_def.type_annotation = type_annotation;
    node->field_def.is_public = is_public;
    return node;
}

// 匹配臂
Node* ast_match_arm(Node* pattern, Node* guard, Node* body, int line, int column) {
    Node* node = node_create(NODE_MATCH_ARM, line, column);
    node->match_arm.pattern = pattern;
    node->match_arm.guard = guard;
    node->match_arm.body = body;
    return node;
}

// 类型注解
Node* ast_type_annotation(Type* type, int line, int column) {
    Node* node = node_create(NODE_TYPE_ANNOTATION, line, column);
    node->type_annotation.type = type;
    return node;
}

// ===== 类型创建函数 =====

Type* type_create_basic(TypeKind kind, const char* name) {
    Type* type = malloc(sizeof(Type));
    if (!type) return NULL;
    
    type->kind = kind;
    type->name = name ? xstrdup(name) : NULL;
    
    return type;
}

Type* type_create_array(Type* element_type, int size) {
    Type* type = type_create_basic(TYPE_ARRAY, NULL);
    type->array.element_type = element_type;
    type->array.array_size = size;
    return type;
}

Type* type_create_slice(Type* element_type) {
    Type* type = type_create_basic(TYPE_SLICE, NULL);
    type->slice.element_type = element_type;
    return type;
}

Type* type_create_tuple(Vector* element_types) {
    Type* type = type_create_basic(TYPE_TUPLE, NULL);
    type->tuple.element_types = element_types ? element_types : vector_create(free_direct);
    return type;
}

Type* type_create_function(Vector* param_types, Type* return_type) {
    Type* type = type_create_basic(TYPE_FUNCTION, NULL);
    type->function.param_types = param_types ? param_types : vector_create(free_direct);
    type->function.return_type = return_type;
    return type;
}

Type* type_create_generic(const char* name, Vector* type_args) {
    Type* type = type_create_basic(TYPE_GENERIC, NULL);
    type->generic.generic_name = xstrdup(name);
    type->generic.type_args = type_args ? type_args : vector_create(free_direct);
    return type;
}

Type* type_create_named(const char* name, Symbol* symbol) {
    Type* type = type_create_basic(TYPE_STRUCT, NULL); // 或 TYPE_ENUM
    type->named.type_name = xstrdup(name);
    type->named.symbol = symbol;
    return type;
}

// ===== 内存释放函数 =====

void type_free(Type* type) {
    if (!type) return;
    
    free(type->name);
    
    switch (type->kind) {
        case TYPE_ARRAY:
            type_free(type->array.element_type);
            break;
        case TYPE_SLICE:
            type_free(type->slice.element_type);
            break;
        case TYPE_TUPLE:
            vector_free(type->tuple.element_types);
            break;
        case TYPE_FUNCTION:
            vector_free(type->function.param_types);
            type_free(type->function.return_type);
            break;
        case TYPE_GENERIC:
            free(type->generic.generic_name);
            vector_free(type->generic.type_args);
            break;
        case TYPE_STRUCT:
        case TYPE_ENUM:
            free(type->named.type_name);
            // 注意：不释放 symbol，它由符号表管理
            break;
        default:
            break;
    }
    
    free(type);
}

// 递归释放节点的辅助函数
static void free_node_recursive(Node* node) {
    if (!node) return;
    
    // 先释放后续节点
    if (node->next) {
        free_node_recursive(node->next);
    }
    
    // 释放节点类型
    if (node->node_type) {
        type_free(node->node_type);
    }
    
    // 根据节点类型释放特定数据
    switch (node->type) {
        case NODE_PROGRAM:
            vector_free(node->program.declarations);
            break;
            
        case NODE_FUNCTION_DECL:
            free(node->func_decl.name);
            vector_free(node->func_decl.generic_params);
            vector_free(node->func_decl.params);
            ast_free(node->func_decl.return_type);
            ast_free(node->func_decl.body);
            break;
            
        case NODE_VAR_DECL:
            free(node->var_decl.name);
            ast_free(node->var_decl.type_annotation);
            ast_free(node->var_decl.value);
            break;
            
        case NODE_STRUCT_DECL:
            free(node->struct_decl.name);
            vector_free(node->struct_decl.generic_params);
            vector_free(node->struct_decl.fields);
            break;
        
        case NODE_FUNCTION_DECL:
            free(node->var_decl.name);
            ast_free(node->var_decl.type_annotation);
            ast_free(node->vardecl.value);
            break;
            
        case NODE_ENUM_DECL:
            free(node->enum_decl.name);
            vector_free(node->enum_decl.variants);
            break;
            
        case NODE_ENUM_VARIANT:
            free(node->enum_variant.name);
            vector_free(node->enum_variant.associated_data);
            break;
            
        case NODE_BLOCK_STMT:
            vector_free(node->block_stmt.statements);
            break;
            
        case NODE_EXPR_STMT:
            ast_free(node->expr_stmt.expression);
            break;
            
        case NODE_IF_STMT:
        case NODE_IF_EXPR:
            ast_free(node->if_stmt.condition);
            ast_free(node->if_stmt.then_branch);
            ast_free(node->if_stmt.else_branch);
            break;
            
        case NODE_FOR_STMT:
            ast_free(node->for_stmt.pattern);
            ast_free(node->for_stmt.iterable);
            ast_free(node->for_stmt.body);
            break;
            
        case NODE_WHILE_STMT:
            ast_free(node->while_stmt.condition);
            ast_free(node->while_stmt.body);
            break;
            
        case NODE_MATCH_STMT:
        case NODE_MATCH_EXPR:
            ast_free(node->match_stmt.subject);
            vector_free(node->match_stmt.arms);
            break;
            
        case NODE_RETURN_STMT:
            ast_free(node->return_stmt.value);
            break;
            
        case NODE_BINARY_EXPR:
            ast_free(node->binary_expr.left);
            ast_free(node->binary_expr.right);
            break;
            
        case NODE_UNARY_EXPR:
            ast_free(node->unary_expr.operand);
            break;
            
        case NODE_CALL_EXPR:
            ast_free(node->call_expr.callee);
            vector_free(node->call_expr.arguments);
            break;
            
        case NODE_METHOD_CALL_EXPR:
            ast_free(node->method_call_expr.receiver);
            free(node->method_call_expr.method_name);
            vector_free(node->method_call_expr.arguments);
            break;
            
        case NODE_FIELD_ACCESS_EXPR:
            ast_free(node->field_access_expr.object);
            free(node->field_access_expr.field_name);
            break;
            
        case NODE_ARRAY_ACCESS_EXPR:
            ast_free(node->array_access_expr.array);
            ast_free(node->array_access_expr.index);
            break;
            
        case NODE_IDENTIFIER_EXPR:
            free(node->identifier_expr.name);
            break;
            
        case NODE_GROUP_EXPR:
            ast_free(node->group_expr.expression);
            break;
            
        case NODE_ARRAY_EXPR:
            vector_free(node->array_expr.elements);
            break;
            
        case NODE_STRUCT_EXPR:
            free(node->struct_expr.struct_name);
            vector_free(node->struct_expr.fields);
            break;
            
        case NODE_TUPLE_EXPR:
            vector_free(node->tuple_expr.elements);
            break;
            
        case NODE_CLOSURE_EXPR:
            vector_free(node->closure_expr.params);
            ast_free(node->closure_expr.return_type);
            ast_free(node->closure_expr.body);
            break;
            
        case NODE_AWAIT_EXPR:
            ast_free(node->await_expr.expression);
            break;
            
        case NODE_PATTERN_IDENT:
            free(node->pattern_ident.name);
            ast_free(node->pattern_ident.subpattern);
            break;
            
        case NODE_PATTERN_LITERAL:
            ast_free(node->pattern_literal.literal);
            break;
            
        case NODE_PATTERN_STRUCT:
            free(node->pattern_struct.struct_name);
            vector_free(node->pattern_struct.field_patterns);
            break;
            
        case NODE_PATTERN_TUPLE:
            vector_free(node->pattern_tuple.element_patterns);
            break;
            
        case NODE_PATTERN_OR:
            vector_free(node->pattern_or.patterns);
            break;
            
        case NODE_PARAM:
            free(node->param.name);
            ast_free(node->param.type_annotation);
            break;
            
        case NODE_FIELD_DEF:
            free(node->field_def.name);
            ast_free(node->field_def.type_annotation);
            break;
            
        case NODE_MATCH_ARM:
            ast_free(node->match_arm.pattern);
            ast_free(node->match_arm.guard);
            ast_free(node->match_arm.body);
            break;
            
        case NODE_TYPE_ANNOTATION:
            type_free(node->type_annotation.type);
            break;
            
        default:
            // 对于没有额外数据的节点类型，不需要特殊处理
            break;
    }
    
    // 最后释放节点本身
    free(node);
}

void ast_free(Node* node) {
    free_node_recursive(node);
}
