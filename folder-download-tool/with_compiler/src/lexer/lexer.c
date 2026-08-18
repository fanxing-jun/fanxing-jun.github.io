// src/lexer/lexer.c
#include "lexer.h"
#include <ctype.h>
#include <stdio.h>

// 关键字映射表
typedef struct {
    const char* keyword;
    TokenType type;
} Keyword;

static Keyword keywords[] = {
    {"with", TOKEN_WITH},
    {"var", TOKEN_VAR},
    {"new", TOKEN_NEW},
    {"async", TOKEN_ASYNC},
    {"await", TOKEN_AWAIT},
    {"return", TOKEN_RETURN},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"for", TOKEN_FOR},
    {"while", TOKEN_WHILE},
    {"attempt", TOKEN_ATTEMPT},
    {"rescue", TOKEN_RESCUE},
    {"shared", TOKEN_SHARED},
    {"pub", TOKEN_PUB},
    {"private", TOKEN_PRIVATE},
    {"protected", TOKEN_PROTECTED},
    {"internal", TOKEN_INTERNAL},
    {"true", TOKEN_BOOLEAN},
    {"false", TOKEN_BOOLEAN},
    {NULL, TOKEN_ERROR}
};

// 检查是否为关键字
static TokenType lexer_check_keyword(const char* str) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

// 创建 Token
Token* token_create(TokenType type, const char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = (char*)malloc(strlen(lexeme) + 1);
    strcpy(token->lexeme, lexeme);
    token->line = line;
    token->column = column;
    
    // 初始化字面量
    token->literal.int_value = 0;
    token->literal.float_value = 0.0;
    token->literal.string_value = NULL;
    token->literal.bool_value = 0;
    
    return token;
}

void token_free(Token* token) {
    if (token) {
        free(token->lexeme);
        if (token->literal.string_value) {
            free(token->literal.string_value);
        }
        free(token);
    }
}

const char* token_type_to_string(TokenType type) {
    switch(type) {
        case TOKEN_WITH: return "WITH";
        case TOKEN_VAR: return "VAR";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

void token_print(Token* token) {
    printf("Token{type: %s, lexeme: '%s', line: %d, col: %d",
           token_type_to_string(token->type),
           token->lexeme,
           token->line,
           token->column);
    
    if (token->type == TOKEN_NUMBER) {
        printf(", value: %g", token->literal.float_value);
    } else if (token->type == TOKEN_STRING) {
        printf(", string: '%s'", token->literal.string_value);
    } else if (token->type == TOKEN_BOOLEAN) {
        printf(", bool: %s", token->literal.bool_value ? "true" : "false");
    }
    
    printf("}\n");
}

// 初始化词法分析器
Lexer* lexer_init(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = lexer->length > 0 ? source[0] : '\0';
    return lexer;
}

void lexer_free(Lexer* lexer) {
    free(lexer);
}

// 前进一个字符
static void lexer_advance(Lexer* lexer) {
    if (lexer->position < lexer->length) {
        lexer->position++;
        if (lexer->position < lexer->length) {
            lexer->current_char = lexer->source[lexer->position];
            lexer->column++;
        } else {
            lexer->current_char = '\0';
        }
    }
}

// 窥视下一个字符
char lexer_peek(Lexer* lexer, int offset) {
    int peek_pos = lexer->position + offset;
    if (peek_pos >= lexer->length) {
        return '\0';
    }
    return lexer->source[peek_pos];
}

// 跳过空白字符
void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->current_char == ' ' || 
           lexer->current_char == '\t' ||
           lexer->current_char == '\r') {
        lexer_advance(lexer);
    }
    
    // 处理换行
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
        lexer_advance(lexer);
        lexer_skip_whitespace(lexer); // 递归跳过后续空白
    }
}

// 读取标识符或关键字
Token* lexer_read_identifier(Lexer* lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column;
    
    // 读取字母、数字、下划线
    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        lexer_advance(lexer);
    }
    
    // 提取标识符字符串
    int length = lexer->position - start_pos;
    char* ident = (char*)malloc(length + 1);
    strncpy(ident, lexer->source + start_pos, length);
    ident[length] = '\0';
    
    // 检查是否为关键字
    TokenType type = lexer_check_keyword(ident);
    
    Token* token = token_create(type, ident, lexer->line, start_col);
    
    // 处理布尔值
    if (type == TOKEN_BOOLEAN) {
        token->literal.bool_value = (strcmp(ident, "true") == 0);
    }
    
    free(ident);
    return token;
}

// 读取多词方法名 (如 "say hello")
Token* lexer_read_multipart_identifier(Lexer* lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column;
    
    // 读取第一个单词
    while (isalnum(lexer->current_char) || lexer->current_char == '_') {
        lexer_advance(lexer);
    }
    
    // 检查后续是否有空格+单词
    while (lexer->current_char == ' ') {
        lexer_advance(lexer); // 跳过空格
        if (isalpha(lexer->current_char)) {
            // 读取下一个单词
            while (isalnum(lexer->current_char) || lexer->current_char == '_') {
                lexer_advance(lexer);
            }
        } else {
            break;
        }
    }
    
    // 提取完整的方法名
    int length = lexer->position - start_pos;
    char* method_name = (char*)malloc(length + 1);
    strncpy(method_name, lexer->source + start_pos, length);
    method_name[length] = '\0';
    
    return token_create(TOKEN_IDENTIFIER, method_name, lexer->line, start_col);
}

// 读取数字（支持整数和浮点数）
Token* lexer_read_number(Lexer* lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column;
    int is_float = 0;
    
    // 读取整数部分
    while (isdigit(lexer->current_char)) {
        lexer_advance(lexer);
    }
    
    // 读取小数部分
    if (lexer->current_char == '.') {
        is_float = 1;
        lexer_advance(lexer);
        while (isdigit(lexer->current_char)) {
            lexer_advance(lexer);
        }
    }
    
    // 提取数字字符串
    int length = lexer->position - start_pos;
    char* num_str = (char*)malloc(length + 1);
    strncpy(num_str, lexer->source + start_pos, length);
    num_str[length] = '\0';
    
    Token* token = token_create(TOKEN_NUMBER, num_str, lexer->line, start_col);
    
    if (is_float) {
        token->literal.float_value = atof(num_str);
    } else {
        token->literal.int_value = atoi(num_str);
        token->literal.float_value = (double)token->literal.int_value;
    }
    
    free(num_str);
    return token;
}

// 读取字符串
Token* lexer_read_string(Lexer* lexer) {
    int start_pos = lexer->position;
    int start_col = lexer->column;
    char quote_char = lexer->current_char; // " 或 '
    
    lexer_advance(lexer); // 跳过开始引号
    
    // 读取字符串内容直到结束引号
    while (lexer->current_char != '\0' && lexer->current_char != quote_char) {
        if (lexer->current_char == '\\') {
            lexer_advance(lexer); // 跳过转义字符
        }
        lexer_advance(lexer);
    }
    
    if (lexer->current_char == '\0') {
        // 未闭合的字符串
        return token_create(TOKEN_ERROR, "Unclosed string", lexer->line, start_col);
    }
    
    lexer_advance(lexer); // 跳过结束引号
    
    // 提取字符串（不包括引号）
    int length = lexer->position - start_pos - 2;
    char* str_content = (char*)malloc(length + 1);
    strncpy(str_content, lexer->source + start_pos + 1, length);
    str_content[length] = '\0';
    
    Token* token = token_create(TOKEN_STRING, str_content, lexer->line, start_col);
    token->literal.string_value = str_content;
    
    return token;
}

// 获取下一个 Token
Token* lexer_next_token(Lexer* lexer) {
    // 跳过空白字符
    lexer_skip_whitespace(lexer);
    
    if (lexer->current_char == '\0') {
        return token_create(TOKEN_EOF, "EOF", lexer->line, lexer->column);
    }
    
    // 识别标识符（包括多词方法名）
    if (isalpha(lexer->current_char) || lexer->current_char == '_') {
        // 检查是否为多词方法名（需要上下文判断，这里简化处理）
        return lexer_read_identifier(lexer);
    }
    
    // 识别数字
    if (isdigit(lexer->current_char)) {
        return lexer_read_number(lexer);
    }
    
    // 识别字符串
    if (lexer->current_char == '"' || lexer->current_char == '\'') {
        return lexer_read_string(lexer);
    }
    
    // 识别操作符和分隔符
    int line = lexer->line;
    int col = lexer->column;
    char c = lexer->current_char;
    
    lexer_advance(lexer);
    
    switch (c) {
        case '(': return token_create(TOKEN_LPAREN, "(", line, col);
        case ')': return token_create(TOKEN_RPAREN, ")", line, col);
        case '{': return token_create(TOKEN_LBRACE, "{", line, col);
        case '}': return token_create(TOKEN_RBRACE, "}", line, col);
        case '[': return token_create(TOKEN_LBRACKET, "[", line, col);
        case ']': return token_create(TOKEN_RBRACKET, "]", line, col);
        case ',': return token_create(TOKEN_COMMA, ",", line, col);
        case '.': return token_create(TOKEN_DOT, ".", line, col);
        case ';': return token_create(TOKEN_SEMICOLON, ";", line, col);
        case ':': return token_create(TOKEN_COLON, ":", line, col);
        case '+': return token_create(TOKEN_PLUS, "+", line, col);
        case '-': 
            if (lexer->current_char == '>') {
                lexer_advance(lexer);
                return token_create(TOKEN_ARROW, "->", line, col);
            }
            return token_create(TOKEN_MINUS, "-", line, col);
        case '*': return token_create(TOKEN_STAR, "*", line, col);
        case '/': return token_create(TOKEN_SLASH, "/", line, col);
        case '=':
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_EQ, "==", line, col);
            }
            return token_create(TOKEN_ASSIGN, "=", line, col);
        case '!':
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_NEQ, "!=", line, col);
            }
            return token_create(TOKEN_NOT, "!", line, col);
        case '<':
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_LTE, "<=", line, col);
            }
            return token_create(TOKEN_LT, "<", line, col);
        case '>':
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_GTE, ">=", line, col);
            }
            return token_create(TOKEN_GT, ">", line, col);
        case '&':
            if (lexer->current_char == '&') {
                lexer_advance(lexer);
                return token_create(TOKEN_AND, "&&", line, col);
            }
            break;
        case '|':
            if (lexer->current_char == '|') {
                lexer_advance(lexer);
                return token_create(TOKEN_OR, "||", line, col);
            }
            break;
    }
    
    // 无法识别的字符
    char error_msg[2] = {c, '\0'};
    return token_create(TOKEN_ERROR, error_msg, line, col);
}