// src/lexer/lexer.h
#ifndef LEXER_H
#define LEXER_H

#include "token.h"

typedef struct {
    const char* source;    // 源代码字符串
    int length;            // 源代码长度
    int position;          // 当前读取位置
    int line;              // 当前行号
    int column;            // 当前列号
    char current_char;     // 当前字符
} Lexer;

// 初始化词法分析器
Lexer* lexer_init(const char* source);

// 释放词法分析器
void lexer_free(Lexer* lexer);

// 获取下一个 Token
Token* lexer_next_token(Lexer* lexer);

// 窥视下一个字符（不移动位置）
char lexer_peek(Lexer* lexer, int offset);

// 跳过空白字符
void lexer_skip_whitespace(Lexer* lexer);

// 读取标识符或关键字
Token* lexer_read_identifier(Lexer* lexer);

// 读取数字
Token* lexer_read_number(Lexer* lexer);

// 读取字符串
Token* lexer_read_string(Lexer* lexer);

// 读取多词方法名
Token* lexer_read_multipart_identifier(Lexer* lexer);

#endif