#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../ast/ast.h"

typedef struct {
    Lexer* lexer;
    Token current;
    Token previous;
    Vector* errors;
    bool panic_mode;  // 错误恢复模式
} Parser;

// 创建和销毁
Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);

// 主要解析函数
Node* parser_parse(Parser* parser);
Node* parser_parse_expression(Parser* parser);
Node* parser_parse_statement(Parser* parser);
Node* parser_parse_declaration(Parser* parser);

// 错误处理
void parser_error(Parser* parser, const char* message);
void parser_error_at(Parser* parser, Token* token, const char* message);
void parser_error_at_current(Parser* parser, const char* message);
void parser_synchronize(Parser* parser);

// 工具函数
bool parser_check(Parser* parser, TokenType type);
bool parser_match(Parser* parser, TokenType type);
Token parser_consume(Parser* parser, TokenType type, const char* error_message);
bool parser_expect(Parser* parser, TokenType type, const char* error_message);
void parser_advance(Parser* parser);

// 获取错误信息
bool parser_has_errors(const Parser* parser);
void parser_print_errors(const Parser* parser);

#endif