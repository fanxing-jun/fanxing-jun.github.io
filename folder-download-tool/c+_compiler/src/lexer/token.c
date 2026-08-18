#include "tokens.h"
#include "token_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建基础 Token
Token token_create(TokenType type, const char* start, int length, int line, int column) {
    Token token;
    token.type = type;
    token.start = start;
    token.length = length;
    token.line = line;
    token.column = column;
    
    // 初始化联合体
    token.int_value = 0;
    token.float_value = 0.0;
    token.char_value = '\0';
    token.string_value = NULL;
    token.error_message = NULL;
    
    return token;
}

// 创建 EOF Token
Token token_create_eof(int line, int column) {
    return token_create(TOKEN_EOF, NULL, 0, line, column);
}

// 创建错误 Token
Token token_create_error(const char* message, int line, int column) {
    Token token = token_create(TOKEN_ERROR, NULL, 0, line, column);
    token.error_message = strdup(message);
    return token;
}

// 释放 Token 资源
void token_free(Token* token) {
    if (!token) return;
    
    if (token->type == TOKEN_STRING && token->string_value) {
        free(token->string_value);
        token->string_value = NULL;
    }
    
    if (token->type == TOKEN_ERROR && token->error_message) {
        free(token->error_message);
        token->error_message = NULL;
    }
}

// Token 类型转字符串
const char* token_type_to_string(TokenType type) {
    static const char* token_names[] = {
        // 字面量
        [TOKEN_EOF] = "EOF",
        [TOKEN_IDENTIFIER] = "IDENTIFIER",
        [TOKEN_INTEGER] = "INTEGER",
        [TOKEN_FLOAT] = "FLOAT",
        [TOKEN_STRING] = "STRING",
        [TOKEN_CHAR] = "CHAR",
        
        // 关键字
        [TOKEN_FUNC] = "func",
        [TOKEN_LET] = "let",
        [TOKEN_MUT] = "mut",
        [TOKEN_CONST] = "const",
        [TOKEN_IF] = "if",
        [TOKEN_ELSE] = "else",
        [TOKEN_FOR] = "for",
        [TOKEN_WHILE] = "while",
        [TOKEN_LOOP] = "loop",
        [TOKEN_MATCH] = "match",
        [TOKEN_RETURN] = "return",
        [TOKEN_BREAK] = "break",
        [TOKEN_CONTINUE] = "continue",
        [TOKEN_STRUCT] = "struct",
        [TOKEN_ENUM] = "enum",
        [TOKEN_IMPL] = "impl",
        [TOKEN_INTERFACE] = "interface",
        [TOKEN_TRAIT] = "trait",
        [TOKEN_MODULE] = "module",
        [TOKEN_IMPORT] = "import",
        [TOKEN_AS] = "as",
        [TOKEN_PUB] = "pub",
        [TOKEN_PRIV] = "priv",
        [TOKEN_ASYNC] = "async",
        [TOKEN_AWAIT] = "await",
        [TOKEN_SPAWN] = "spawn",
        [TOKEN_TYPE] = "type",
        [TOKEN_SELF] = "self",
        [TOKEN_SUPER] = "super",
        
        // 类型关键字
        [TOKEN_INT] = "int",
        [TOKEN_INT8] = "int8",
        [TOKEN_INT16] = "int16",
        [TOKEN_INT32] = "int32",
        [TOKEN_INT64] = "int64",
        [TOKEN_UINT] = "uint",
        [TOKEN_UINT8] = "uint8",
        [TOKEN_UINT16] = "uint16",
        [TOKEN_UINT32] = "uint32",
        [TOKEN_UINT64] = "uint64",
        [TOKEN_FLOAT32] = "float32",
        [TOKEN_FLOAT64] = "float64",
        [TOKEN_BOOL] = "bool",
        [TOKEN_CHAR_TYPE] = "char",
        [TOKEN_STRING_TYPE] = "string",
        [TOKEN_VOID] = "void",
        
        // 布尔字面量
        [TOKEN_TRUE] = "true",
        [TOKEN_FALSE] = "false",
        
        // 运算符
        [TOKEN_PLUS] = "+",
        [TOKEN_MINUS] = "-",
        [TOKEN_STAR] = "*",
        [TOKEN_SLASH] = "/",
        [TOKEN_PERCENT] = "%",
        [TOKEN_EQ] = "=",
        [TOKEN_PLUS_EQ] = "+=",
        [TOKEN_MINUS_EQ] = "-=",
        [TOKEN_STAR_EQ] = "*=",
        [TOKEN_SLASH_EQ] = "/=",
        [TOKEN_PERCENT_EQ] = "%=",
        [TOKEN_EQEQ] = "==",
        [TOKEN_NEQ] = "!=",
        [TOKEN_LT] = "<",
        [TOKEN_GT] = ">",
        [TOKEN_LTEQ] = "<=",
        [TOKEN_GTEQ] = ">=",
        [TOKEN_AND] = "&&",
        [TOKEN_OR] = "||",
        [TOKEN_NOT] = "!",
        [TOKEN_AMP] = "&",
        [TOKEN_PIPE] = "|",
        [TOKEN_CARET] = "^",
        [TOKEN_TILDE] = "~",
        [TOKEN_LSHIFT] = "<<",
        [TOKEN_RSHIFT] = ">>",
        [TOKEN_QUESTION] = "?",
        [TOKEN_DOT] = ".",
        [TOKEN_DOTDOT] = "..",
        [TOKEN_DOTDOTDOT] = "...",
        [TOKEN_ARROW] = "->",
        [TOKEN_FAT_ARROW] = "=>",
        [TOKEN_AT] = "@",
        
        // 标点符号
        [TOKEN_LPAREN] = "(",
        [TOKEN_RPAREN] = ")",
        [TOKEN_LBRACE] = "{",
        [TOKEN_RBRACE] = "}",
        [TOKEN_LBRACKET] = "[",
        [TOKEN_RBRACKET] = "]",
        [TOKEN_COMMA] = ",",
        [TOKEN_COLON] = ":",
        [TOKEN_SEMICOLON] = ";",
        [TOKEN_DOUBLE_COLON] = "::",
        
        // 特殊
        [TOKEN_ERROR] = "ERROR",
        [TOKEN_COMMENT] = "COMMENT",
        [TOKEN_LINE_COMMENT] = "LINE_COMMENT",
        [TOKEN_HASH] = "#",
        [TOKEN_DOLLAR] = "$",
        [TOKEN_AMP_MUT] = "&mut",
        [TOKEN_AMP_OWNED] = "&owned",
    };
    
    if (type >= 0 && type < _TOKEN_TYPE_COUNT) {
        return token_names[type];
    }
    
    return "UNKNOWN";
}

// 检查 Token 类型
bool token_is_keyword(TokenType type) {
    return (type >= TOKEN_FUNC && type <= TOKEN_SUPER) ||
           (type >= TOKEN_INT && type <= TOKEN_VOID) ||
           type == TOKEN_TRUE || type == TOKEN_FALSE;
}

bool token_is_operator(TokenType type) {
    return (type >= TOKEN_PLUS && type <= TOKEN_AT) ||
           type == TOKEN_AMP_MUT || type == TOKEN_AMP_OWNED;
}

bool token_is_literal(TokenType type) {
    return type == TOKEN_INTEGER || type == TOKEN_FLOAT || 
           type == TOKEN_STRING || type == TOKEN_CHAR ||
           type == TOKEN_TRUE || type == TOKEN_FALSE;
}

bool token_is_type_keyword(TokenType type) {
    return type >= TOKEN_INT && type <= TOKEN_VOID;
}

// Token 比较
bool token_equals(const Token* token, const char* str) {
    if (!token || !str) return false;
    if (token->length != (int)strlen(str)) return false;
    return strncmp(token->start, str, token->length) == 0;
}

bool token_equals_type(const Token* token, TokenType type) {
    return token && token->type == type;
}

bool token_is_eof(const Token* token) {
    return token && token->type == TOKEN_EOF;
}

bool token_is_error(const Token* token) {
    return token && token->type == TOKEN_ERROR;
}