#include "lexer.h"
#include "token_utils.h"
#include "../utils/utils.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// 创建词法分析器
Lexer* lexer_create(const char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->errors = vector_create(free_direct);
    lexer->had_error = false;
    
    return lexer;
}

// 释放词法分析器
void lexer_free(Lexer* lexer) {
    if (!lexer) return;
    
    vector_free(lexer->errors);
    free(lexer);
}

// 前进一个字符
char lexer_advance(Lexer* lexer) {
    if (lexer_is_at_end(lexer)) {
        return '\0';
    }
    
    char c = *lexer->current;
    lexer->current++;
    
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    return c;
}

// 查看当前字符
char lexer_peek(const Lexer* lexer) {
    if (lexer_is_at_end(lexer)) {
        return '\0';
    }
    return *lexer->current;
}

// 查看下一个字符
char lexer_peek_next(const Lexer* lexer) {
    if (lexer_is_at_end(lexer)) {
        return '\0';
    }
    return *(lexer->current + 1);
}

// 检查是否到达文件末尾
bool lexer_is_at_end(const Lexer* lexer) {
    return *lexer->current == '\0';
}

// 匹配并前进（如果匹配）
bool lexer_match(Lexer* lexer, char expected) {
    if (lexer_is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    
    lexer_advance(lexer);
    return true;
}

// 跳过空白字符
static void lexer_skip_whitespace(Lexer* lexer) {
    while (!lexer_is_at_end(lexer)) {
        char c = lexer_peek(lexer);
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                lexer_advance(lexer);
                break;
            case '\n':
                lexer_advance(lexer);
                break;
            default:
                return;
        }
    }
}

// 跳过注释
static void lexer_skip_comment(Lexer* lexer) {
    // 行注释
    if (lexer_match(lexer, '/') && lexer_match(lexer, '/')) {
        while (!lexer_is_at_end(lexer) && lexer_peek(lexer) != '\n') {
            lexer_advance(lexer);
        }
    }
    // 块注释
    else if (lexer_match(lexer, '/') && lexer_match(lexer, '*')) {
        int depth = 1;
        while (!lexer_is_at_end(lexer) && depth > 0) {
            if (lexer_peek(lexer) == '*' && lexer_peek_next(lexer) == '/') {
                lexer_advance(lexer); // *
                lexer_advance(lexer); // /
                depth--;
            } else if (lexer_peek(lexer) == '/' && lexer_peek_next(lexer) == '*') {
                lexer_advance(lexer); // /
                lexer_advance(lexer); // *
                depth++;
            } else {
                lexer_advance(lexer);
            }
        }
        
        if (depth > 0) {
            lexer_add_error(lexer, "Unterminated block comment", lexer->line, lexer->column);
        }
    } else {
        // 不是注释，回退
        lexer->current -= 2;
        lexer->column -= 2;
    }
}

// 读取字符串字面量
static Token lexer_read_string(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    
    // 跳过开头的 "
    lexer_advance(lexer);
    
    StringBuilder* sb = sb_create();
    while (!lexer_is_at_end(lexer) && lexer_peek(lexer) != '"') {
        char c = lexer_advance(lexer);
        
        // 处理转义字符
        if (c == '\\') {
            if (lexer_is_at_end(lexer)) {
                sb_free(sb);
                return token_create_error("Unterminated string", start_line, start_column);
            }
            
            char escape = lexer_advance(lexer);
            switch (escape) {
                case 'n': sb_append_char(sb, '\n'); break;
                case 't': sb_append_char(sb, '\t'); break;
                case 'r': sb_append_char(sb, '\r'); break;
                case '\\': sb_append_char(sb, '\\'); break;
                case '"': sb_append_char(sb, '"'); break;
                case '\'': sb_append_char(sb, '\''); break;
                case '0': sb_append_char(sb, '\0'); break;
                default:
                    sb_free(sb);
                    return token_create_error("Invalid escape sequence", lexer->line, lexer->column);
            }
        } else {
            sb_append_char(sb, c);
        }
    }
    
    if (lexer_is_at_end(lexer)) {
        sb_free(sb);
        return token_create_error("Unterminated string", start_line, start_column);
    }
    
    // 跳过结尾的 "
    lexer_advance(lexer);
    
    Token token = token_create(TOKEN_STRING, lexer->start, 
                              lexer->current - lexer->start, 
                              start_line, start_column);
    token.string_value = sb_to_string(sb);
    sb_free(sb);
    
    return token;
}

// 读取字符字面量
static Token lexer_read_char(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    
    // 跳过开头的 '
    lexer_advance(lexer);
    
    if (lexer_is_at_end(lexer)) {
        return token_create_error("Unterminated character", start_line, start_column);
    }
    
    char c = lexer_advance(lexer);
    char value;
    
    // 处理转义字符
    if (c == '\\') {
        if (lexer_is_at_end(lexer)) {
            return token_create_error("Unterminated character", start_line, start_column);
        }
        
        char escape = lexer_advance(lexer);
        switch (escape) {
            case 'n': value = '\n'; break;
            case 't': value = '\t'; break;
            case 'r': value = '\r'; break;
            case '\\': value = '\\'; break;
            case '\'': value = '\''; break;
            case '"': value = '"'; break;
            case '0': value = '\0'; break;
            default:
                return token_create_error("Invalid escape sequence", lexer->line, lexer->column);
        }
    } else {
        value = c;
    }
    
    if (lexer_peek(lexer) != '\'') {
        return token_create_error("Unterminated character", start_line, start_column);
    }
    
    // 跳过结尾的 '
    lexer_advance(lexer);
    
    Token token = token_create(TOKEN_CHAR, lexer->start, 
                              lexer->current - lexer->start, 
                              start_line, start_column);
    token.char_value = value;
    return token;
}

// 读取数字字面量
static Token lexer_read_number(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    
    bool is_float = false;
    
    // 读取整数部分
    while (isdigit(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }
    
    // 检查小数部分
    if (lexer_peek(lexer) == '.' && isdigit(lexer_peek_next(lexer))) {
        is_float = true;
        lexer_advance(lexer); // 跳过 .
        
        while (isdigit(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }
    }
    
    // 检查科学计数法
    if (lexer_peek(lexer) == 'e' || lexer_peek(lexer) == 'E') {
        is_float = true;
        lexer_advance(lexer); // 跳过 e/E
        
        if (lexer_peek(lexer) == '+' || lexer_peek(lexer) == '-') {
            lexer_advance(lexer);
        }
        
        if (!isdigit(lexer_peek(lexer))) {
            return token_create_error("Invalid number format", start_line, start_column);
        }
        
        while (isdigit(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }
    }
    
    // 提取数字字符串并转换
    int length = lexer->current - lexer->start;
    char* number_str = malloc(length + 1);
    strncpy(number_str, lexer->start, length);
    number_str[length] = '\0';
    
    Token token;
    if (is_float) {
        token = token_create(TOKEN_FLOAT, lexer->start, length, start_line, start_column);
        token.float_value = atof(number_str);
    } else {
        token = token_create(TOKEN_INTEGER, lexer->start, length, start_line, start_column);
        token.int_value = atol(number_str);
    }
    
    free(number_str);
    return token;
}

// 读取标识符或关键字
static Token lexer_read_identifier(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    
    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
        lexer_advance(lexer);
    }
    
    int length = lexer->current - lexer->start;
    TokenType type = keyword_from_string(lexer->start, length);
    
    return token_create(type, lexer->start, length, start_line, start_column);
}

// 读取运算符或标点符号
static Token lexer_read_operator(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    
    char c = lexer_advance(lexer);
    
    switch (c) {
        // 单字符运算符
        case '(': return token_create(TOKEN_LPAREN, lexer->start, 1, start_line, start_column);
        case ')': return token_create(TOKEN_RPAREN, lexer->start, 1, start_line, start_column);
        case '{': return token_create(TOKEN_LBRACE, lexer->start, 1, start_line, start_column);
        case '}': return token_create(TOKEN_RBRACE, lexer->start, 1, start_line, start_column);
        case '[': return token_create(TOKEN_LBRACKET, lexer->start, 1, start_line, start_column);
        case ']': return token_create(TOKEN_RBRACKET, lexer->start, 1, start_line, start_column);
        case ',': return token_create(TOKEN_COMMA, lexer->start, 1, start_line, start_column);
        case ';': return token_create(TOKEN_SEMICOLON, lexer->start, 1, start_line, start_column);
        case '@': return token_create(TOKEN_AT, lexer->start, 1, start_line, start_column);
        case '?': return token_create(TOKEN_QUESTION, lexer->start, 1, start_line, start_column);
        case '~': return token_create(TOKEN_TILDE, lexer->start, 1, start_line, start_column);
        
        // 可能的多字符运算符
        case '=':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_EQEQ, lexer->start, 2, start_line, start_column);
            } else if (lexer_match(lexer, '>')) {
                return token_create(TOKEN_FAT_ARROW, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_EQ, lexer->start, 1, start_line, start_column);
            }
            
        case '!':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_NEQ, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_NOT, lexer->start, 1, start_line, start_column);
            }
            
        case '<':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_LTEQ, lexer->start, 2, start_line, start_column);
            } else if (lexer_match(lexer, '<')) {
                return token_create(TOKEN_LSHIFT, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_LT, lexer->start, 1, start_line, start_column);
            }
            
        case '>':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_GTEQ, lexer->start, 2, start_line, start_column);
            } else if (lexer_match(lexer, '>')) {
                return token_create(TOKEN_RSHIFT, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_GT, lexer->start, 1, start_line, start_column);
            }
            
        case '&':
            if (lexer_match(lexer, '&')) {
                return token_create(TOKEN_AND, lexer->start, 2, start_line, start_column);
            } else if (lexer_match(lexer, 'm') && lexer_match(lexer, 'u') && lexer_match(lexer, 't')) {
                return token_create(TOKEN_AMP_MUT, lexer->start, 4, start_line, start_column);
            } else if (lexer_match(lexer, 'o') && lexer_match(lexer, 'w') && lexer_match(lexer, 'n') &&
                      lexer_match(lexer, 'e') && lexer_match(lexer, 'd')) {
                return token_create(TOKEN_AMP_OWNED, lexer->start, 6, start_line, start_column);
            } else {
                return token_create(TOKEN_AMP, lexer->start, 1, start_line, start_column);
            }
            
        case '|':
            if (lexer_match(lexer, '|')) {
                return token_create(TOKEN_OR, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_PIPE, lexer->start, 1, start_line, start_column);
            }
            
        case '+':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_PLUS_EQ, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_PLUS, lexer->start, 1, start_line, start_column);
            }
            
        case '-':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_MINUS_EQ, lexer->start, 2, start_line, start_column);
            } else if (lexer_match(lexer, '>')) {
                return token_create(TOKEN_ARROW, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_MINUS, lexer->start, 1, start_line, start_column);
            }
            
        case '*':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_STAR_EQ, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_STAR, lexer->start, 1, start_line, start_column);
            }
            
        case '/':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_SLASH_EQ, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_SLASH, lexer->start, 1, start_line, start_column);
            }
            
        case '%':
            if (lexer_match(lexer, '=')) {
                return token_create(TOKEN_PERCENT_EQ, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_PERCENT, lexer->start, 1, start_line, start_column);
            }
            
        case ':':
            if (lexer_match(lexer, ':')) {
                return token_create(TOKEN_DOUBLE_COLON, lexer->start, 2, start_line, start_column);
            } else {
                return token_create(TOKEN_COLON, lexer->start, 1, start_line, start_column);
            }
            
        case '.':
            if (lexer_match(lexer, '.')) {
                if (lexer_match(lexer, '.')) {
                    return token_create(TOKEN_DOTDOTDOT, lexer->start, 3, start_line, start_column);
                } else {
                    return token_create(TOKEN_DOTDOT, lexer->start, 2, start_line, start_column);
                }
            } else {
                return token_create(TOKEN_DOT, lexer->start, 1, start_line, start_column);
            }
            
        case '#':
            return token_create(TOKEN_HASH, lexer->start, 1, start_line, start_column);
        case '$':
            return token_create(TOKEN_DOLLAR, lexer->start, 1, start_line, start_column);
        case '^':
            return token_create(TOKEN_CARET, lexer->start, 1, start_line, start_column);
            
        default:
            return token_create_error("Unexpected character", start_line, start_column);
    }
}

// 读取下一个token
Token lexer_next_token(Lexer* lexer) {
    lexer_skip_whitespace(lexer);
    
    lexer->start = lexer->current;
    
    if (lexer_is_at_end(lexer)) {
        return token_create_eof(lexer->line, lexer->column);
    }
    
    char c = lexer_peek(lexer);
    
    // 检查注释
    if (c == '/' && (lexer_peek_next(lexer) == '/' || lexer_peek_next(lexer) == '*')) {
        lexer_skip_comment(lexer);
        return lexer_next_token(lexer); // 递归调用跳过注释后继续
    }
    
    // 标识符或关键字
    if (isalpha(c) || c == '_') {
        return lexer_read_identifier(lexer);
    }
    
    // 数字
    if (isdigit(c)) {
        return lexer_read_number(lexer);
    }
    
    // 字符串字面量
    if (c == '"') {
        return lexer_read_string(lexer);
    }
    
    // 字符字面量
    if (c == '\'') {
        return lexer_read_char(lexer);
    }
    
    // 运算符和标点符号
    return lexer_read_operator(lexer);
}

// 一次性tokenize整个文件
Vector* lexer_tokenize(Lexer* lexer) {
    Vector* tokens = vector_create((void (*)(void*))token_free);
    
    while (true) {
        Token token = lexer_next_token(lexer);
        vector_push(tokens, malloc(sizeof(Token)));
        memcpy(vector_get(tokens, vector_size(tokens) - 1), &token, sizeof(Token));
        
        if (token.type == TOKEN_EOF || token.type == TOKEN_ERROR) {
            break;
        }
    }
    
    return tokens;
}