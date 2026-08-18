#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"
#include "../utils/vector.h"

typedef struct {
    const char* source;        // 源代码字符串
    const char* start;         // 当前token起始位置
    const char* current;       // 当前扫描位置
    int line;                  // 当前行号
    int column;                // 当前列号
    Vector* errors;            // 错误列表
    bool had_error;            // 是否有错误
} Lexer;

// 创建和销毁
Lexer* lexer_create(const char* source);
void lexer_free(Lexer* lexer);

// 主要词法分析函数
Token lexer_next_token(Lexer* lexer);
Vector* lexer_tokenize(Lexer* lexer);  // 一次性tokenize整个文件

// 错误处理
void lexer_add_error(Lexer* lexer, const char* message, int line, int column);
bool lexer_has_errors(const Lexer* lexer);
void lexer_print_errors(const Lexer* lexer);

// 工具函数
char lexer_advance(Lexer* lexer);
char lexer_peek(const Lexer* lexer);
char lexer_peek_next(const Lexer* lexer);
bool lexer_is_at_end(const Lexer* lexer);
bool lexer_match(Lexer* lexer, char expected);

#endif