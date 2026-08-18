// src/lexer/token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token 类型枚举
typedef enum {
    // 关键字
    TOKEN_WITH,      // "with"
    TOKEN_VAR,       // "var"
    TOKEN_NEW,       // "new"
    TOKEN_ASYNC,     // "async"
    TOKEN_AWAIT,     // "await"
    TOKEN_RETURN,    // "return"
    TOKEN_IF,        // "if"
    TOKEN_ELSE,      // "else"
    TOKEN_FOR,       // "for"
    TOKEN_WHILE,     // "while"
    TOKEN_ATTEMPT,   // "attempt"
    TOKEN_RESCUE,    // "rescue"
    TOKEN_SHARED,    // "shared"
    TOKEN_PUB,       // "pub"
    TOKEN_PRIVATE,   // "private"
    TOKEN_PROTECTED, // "protected"
    TOKEN_INTERNAL,  // "internal"
    
    // 字面量
    TOKEN_IDENTIFIER,  // 标识符
    TOKEN_STRING,      // 字符串 "hello"
    TOKEN_NUMBER,      // 数字 123, 45.67
    TOKEN_BOOLEAN,     // true/false
    
    // 操作符
    TOKEN_ARROW,       // ->
    TOKEN_ASSIGN,      // =
    TOKEN_PLUS,        // +
    TOKEN_MINUS,       // -
    TOKEN_STAR,        // *
    TOKEN_SLASH,       // /
    TOKEN_EQ,          // ==
    TOKEN_NEQ,         // !=
    TOKEN_LT,          // <
    TOKEN_GT,          // >
    TOKEN_LTE,         // <=
    TOKEN_GTE,         // >=
    TOKEN_AND,         // &&
    TOKEN_OR,          // ||
    TOKEN_NOT,         // !
    
    // 分隔符
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_LBRACKET,    // [
    TOKEN_RBRACKET,    // ]
    TOKEN_COMMA,       // ,
    TOKEN_DOT,         // .
    TOKEN_SEMICOLON,   // ;
    TOKEN_COLON,       // :
    
    // 特殊
    TOKEN_EOF,         // 文件结束
    TOKEN_ERROR        // 错误
} TokenType;

// Token 结构体
typedef struct {
    TokenType type;
    char* lexeme;      // 原始字符串
    int line;          // 行号
    int column;        // 列号
    union {
        int int_value;
        double float_value;
        char* string_value;
        int bool_value;
    } literal;         // 字面量值
} Token;

// 用来创建 Token
Token* token_create(TokenType type, const char* lexeme, int line, int column);

// 释放 Token
void token_free(Token* token);

// 获取 Token 类型的字符串表示
const char* token_type_to_string(TokenType type);

// 打印 Token
void token_print(Token* token);

#endif