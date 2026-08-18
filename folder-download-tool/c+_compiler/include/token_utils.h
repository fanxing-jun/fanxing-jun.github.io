#ifndef TOKEN_UTILS_H
#define TOKEN_UTILS_H

#include "tokens.h"

// Token 创建和销毁
Token token_create(TokenType type, const char* start, int length, int line, int column);
Token token_create_eof(int line, int column);
Token token_create_error(const char* message, int line, int column);
void token_free(Token* token);

// Token 信息查询
const char* token_type_to_string(TokenType type);
bool token_is_keyword(TokenType type);
bool token_is_operator(TokenType type);
bool token_is_literal(TokenType type);
bool token_is_type_keyword(TokenType type);

// Token 比较和检查
bool token_equals(const Token* token, const char* str);
bool token_equals_type(const Token* token, TokenType type);
bool token_is_eof(const Token* token);
bool token_is_error(const Token* token);

// 关键字查找
TokenType keyword_from_string(const char* str, int length);

// 调试输出
void token_print(const Token* token);
char* token_to_string(const Token* token);

#endif