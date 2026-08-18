#include "parser.h"
#include "expression.h"
#include "statement.h"
#include "declaration.h"
#include "pattern.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 创建解析器
Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->errors = vector_create(free_direct);
    parser->panic_mode = false;
    
    // 读取第一个token
    parser_advance(parser);
    
    return parser;
}

// 释放解析器
void parser_free(Parser* parser) {
    if (!parser) return;
    
    // 注意：不释放lexer，由调用者管理
    vector_free(parser->errors);
    free(parser);
}

// 前进到下一个token
void parser_advance(Parser* parser) {
    parser->previous = parser->current;
    
    // 错误恢复：跳过token直到找到同步点
    while (true) {
        parser->current = lexer_next_token(parser->lexer);
        
        if (parser->current.type != TOKEN_ERROR) {
            break;
        }
        
        parser_error_at_current(parser, parser->current.error_message);
    }
}

// 检查当前token类型
bool parser_check(Parser* parser, TokenType type) {
    return parser->current.type == type;
}

// 匹配并前进当前token
bool parser_match(Parser* parser, TokenType type) {
    if (!parser_check(parser, type)) {
        return false;
    }
    parser_advance(parser);
    return true;
}

// 消费指定类型的token，如果类型不匹配则报错
Token parser_consume(Parser* parser, TokenType type, const char* error_message) {
    if (parser_check(parser, type)) {
        Token token = parser->current;
        parser_advance(parser);
        return token;
    }
    
    parser_error_at_current(parser, error_message);
    
    // 返回一个错误token作为占位符
    Token error_token;
    error_token.type = TOKEN_ERROR;
    error_token.start = "";
    error_token.length = 0;
    error_token.line = parser->current.line;
    error_token.column = parser->current.column;
    return error_token;
}

// 期望当前token是指定类型
bool parser_expect(Parser* parser, TokenType type, const char* error_message) {
    if (parser_check(parser, type)) {
        return true;
    }
    
    parser_error_at_current(parser, error_message);
    return false;
}

// 错误处理
void parser_error(Parser* parser, const char* message) {
    parser_error_at(parser, &parser->previous, message);
}

void parser_error_at(Parser* parser, Token* token, const char* message) {
    if (parser->panic_mode) return;
    
    parser->panic_mode = true;
    
    StringBuilder* sb = sb_create();
    if (token->type == TOKEN_EOF) {
        sb_append_format(sb, "[Line %d] Error at end: %s", token->line, message);
    } else {
        sb_append_format(sb, "[Line %d, Column %d] Error at '", token->line, token->column);
        
        // 添加token文本
        for (int i = 0; i < token->length; i++) {
            sb_append_char(sb, token->start[i]);
        }
        
        sb_append_format(sb, "': %s", message);
    }
    
    char* error_msg = sb_to_string(sb);
    vector_push(parser->errors, error_msg);
    sb_free(sb);
}

void parser_error_at_current(Parser* parser, const char* message) {
    parser_error_at(parser, &parser->current, message);
}

// 错误恢复：跳过token直到找到声明或语句的边界
void parser_synchronize(Parser* parser) {
    parser->panic_mode = false;
    
    while (!parser_check(parser, TOKEN_EOF)) {
        if (parser->previous.type == TOKEN_SEMICOLON) {
            return;
        }
        
        switch (parser->current.type) {
            case TOKEN_FUNC:
            case TOKEN_LET:
            case TOKEN_STRUCT:
            case TOKEN_ENUM:
            case TOKEN_IMPL:
            case TOKEN_IF:
            case TOKEN_FOR:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
                return;
            default:
                break;
        }
        
        parser_advance(parser);
    }
}

// 检查是否有错误
bool parser_has_errors(const Parser* parser) {
    return parser && !vector_is_empty(parser->errors);
}

// 打印所有错误
void parser_print_errors(const Parser* parser) {
    if (!parser || vector_is_empty(parser->errors)) {
        return;
    }
    
    printf("Parser Errors:\n");
    for (size_t i = 0; i < vector_size(parser->errors); i++) {
        char* error = vector_get(parser->errors, i);
        printf("  %s\n", error);
    }
}

// 解析整个程序
Node* parser_parse(Parser* parser) {
    Vector* declarations = vector_create(free_direct);
    
    while (!parser_check(parser, TOKEN_EOF)) {
        Node* declaration = parser_parse_declaration(parser);
        if (declaration) {
            vector_push(declarations, declaration);
        }
        
        // 错误恢复
        if (parser->panic_mode) {
            parser_synchronize(parser);
        }
    }
    
    return ast_program(declarations, 1, 1);
}