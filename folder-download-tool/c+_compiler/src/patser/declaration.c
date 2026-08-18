#include "declaration.h"
#include "statement.h"
#include "expression.h"
#include "../ast/ast_utils.h"
#include <stdlib.h>

// 解析声明
Node* parse_declaration(Parser* parser) {
    if (parser_match(parser, TOKEN_FUNC)) {
        return parse_function_declaration(parser);
    }
    if (parser_match(parser, TOKEN_LET)) {
        return parse_variable_declaration(parser);
    }
    if (parser_match(parser, TOKEN_STRUCT)) {
        return parse_struct_declaration(parser);
    }
    if (parser_match(parser, TOKEN_ENUM)) {
        return parse_enum_declaration(parser);
    }
    if (parser_match(parser, TOKEN_IMPL)) {
        return parse_impl_declaration(parser);
    }
    
    // 如果不是声明，尝试解析语句
    return parse_statement(parser);
}

// 解析函数声明
Node* parse_function_declaration(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    bool is_public = false;
    bool is_async = false;
    
    // 解析修饰符
    if (parser_match(parser, TOKEN_PUB)) {
        is_public = true;
    }
    if (parser_match(parser, TOKEN_ASYNC)) {
        is_async = true;
    }
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");
    char* func_name = strndup(name.start, name.length);
    
    // 解析泛型参数（可选）
    Vector* generic_params = NULL;
    if (parser_match(parser, TOKEN_LT)) {
        generic_params = parse_generic_parameters(parser);
    }
    
    // 解析参数列表
    parser_consume(parser, TOKEN_LPAREN, "Expected '(' after function name");
    Vector* params = parse_parameter_list(parser);
    parser_consume(parser, TOKEN_RPAREN, "Expected ')' after parameters");
    
    // 解析返回类型（可选）
    Node* return_type = NULL;
    if (parser_match(parser, TOKEN_ARROW)) {
        return_type = parse_type_annotation(parser);
    } else {
        // 默认返回void
        Type* void_type = type_create_basic(TYPE_VOID, "void");
        return_type = ast_type_annotation(void_type, parser->current.line, parser->current.column);
    }
    
    // 解析函数体
    parser_consume(parser, TOKEN_LBRACE, "Expected '{' before function body");
    Node* body = parse_block_statement(parser);
    
    return ast_function_decl(func_name, is_public, is_async, generic_params, 
                           params, return_type, body, line, column);
}

// 解析变量声明
Node* parse_variable_declaration(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    bool is_mutable = false;
    bool is_public = false;
    
    // 解析修饰符
    if (parser_match(parser, TOKEN_PUB)) {
        is_public = true;
    }
    if (parser_match(parser, TOKEN_MUT)) {
        is_mutable = true;
    }
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    char* var_name = strndup(name.start, name.length);
    
    // 解析类型注解（可选）
    Node* type_annotation = NULL;
    if (parser_match(parser, TOKEN_COLON)) {
        type_annotation = parse_type_annotation(parser);
    }
    
    // 解析初始值（可选）
    Node* value = NULL;
    if (parser_match(parser, TOKEN_EQ)) {
        value = parse_expression(parser);
    }
    
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
    
    return ast_var_decl(var_name, is_mutable, is_public, type_annotation, value, line, column);
}

// 解析结构体声明
Node* parse_struct_declaration(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    bool is_public = false;
    if (parser_match(parser, TOKEN_PUB)) {
        is_public = true;
    }
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected struct name");
    char* struct_name = strndup(name.start, name.length);
    
    // 解析泛型参数（可选）
    Vector* generic_params = NULL;
    if (parser_match(parser, TOKEN_LT)) {
        generic_params = parse_generic_parameters(parser);
    }
    
    parser_consume(parser, TOKEN_LBRACE, "Expected '{' before struct body");
    
    Vector* fields = vector_create(free_direct);
    
    while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        bool field_public = false;
        if (parser_match(parser, TOKEN_PUB)) {
            field_public = true;
        }
        
        Token field_name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name");
        parser_consume(parser, TOKEN_COLON, "Expected ':' after field name");
        
        Node* field_type = parse_type_annotation(parser);
        parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after field type");
        
        char* field_name_str = strndup(field_name.start, field_name.length);
        Node* field = ast_field_def(field_name_str, field_type, field_public, 
                                  field_name.line, field_name.column);
        vector_push(fields, field);
    }
    
    parser_consume(parser, TOKEN_RBRACE, "Expected '}' after struct fields");
    
    return ast_struct_decl(struct_name, is_public, generic_params, fields, line, column);
}

// 解析参数列表
Vector* parse_parameter_list(Parser* parser) {
    Vector* params = vector_create(free_direct);
    
    if (!parser_check(parser, TOKEN_RPAREN)) {
        do {
            bool is_mutable = parser_match(parser, TOKEN_MUT);
            
            Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
            parser_consume(parser, TOKEN_COLON, "Expected ':' after parameter name");
            
            Node* type_annotation = parse_type_annotation(parser);
            
            char* param_name = strndup(name.start, name.length);
            Node* param = ast_param(param_name, type_annotation, is_mutable, 
                                  name.line, name.column);
            vector_push(params, param);
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    return params;
}

// 解析泛型参数（简化版本）
Vector* parse_generic_parameters(Parser* parser) {
    Vector* generics = vector_create(free_direct);
    
    if (!parser_check(parser, TOKEN_GT)) {
        do {
            Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected generic parameter name");
            char* generic_name = strndup(name.start, name.length);
            
            // 创建简单的类型注解节点表示泛型参数
            Type* type = type_create_generic(generic_name, NULL);
            Node* type_annotation = ast_type_annotation(type, name.line, name.column);
            
            vector_push(generics, type_annotation);
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_GT, "Expected '>' after generic parameters");
    return generics;
}