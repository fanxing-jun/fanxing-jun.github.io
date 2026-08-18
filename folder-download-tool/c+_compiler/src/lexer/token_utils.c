#include "token_utils.h"
#include <string.h>
#include <stdio.h>

// 关键字查找表
typedef struct {
    const char* keyword;
    TokenType type;
} KeywordEntry;

static const KeywordEntry keyword_table[] = {
    // 控制流关键字
    {"func", TOKEN_FUNC},
    {"let", TOKEN_LET},
    {"mut", TOKEN_MUT},
    {"const", TOKEN_CONST},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"for", TOKEN_FOR},
    {"while", TOKEN_WHILE},
    {"loop", TOKEN_LOOP},
    {"match", TOKEN_MATCH},
    {"return", TOKEN_RETURN},
    {"break", TOKEN_BREAK},
    {"continue", TOKEN_CONTINUE},
    
    // 类型定义关键字
    {"struct", TOKEN_STRUCT},
    {"enum", TOKEN_ENUM},
    {"impl", TOKEN_IMPL},
    {"interface", TOKEN_INTERFACE},
    {"trait", TOKEN_TRAIT},
    {"type", TOKEN_TYPE},
    
    // 模块系统
    {"module", TOKEN_MODULE},
    {"import", TOKEN_IMPORT},
    {"as", TOKEN_AS},
    {"pub", TOKEN_PUB},
    {"priv", TOKEN_PRIV},
    
    // 并发
    {"async", TOKEN_ASYNC},
    {"await", TOKEN_AWAIT},
    {"spawn", TOKEN_SPAWN},
    
    // 自引用
    {"self", TOKEN_SELF},
    {"super", TOKEN_SUPER},
    
    // 类型关键字
    {"int", TOKEN_INT},
    {"int8", TOKEN_INT8},
    {"int16", TOKEN_INT16},
    {"int32", TOKEN_INT32},
    {"int64", TOKEN_INT64},
    {"uint", TOKEN_UINT},
    {"uint8", TOKEN_UINT8},
    {"uint16", TOKEN_UINT16},
    {"uint32", TOKEN_UINT32},
    {"uint64", TOKEN_UINT64},
    {"float32", TOKEN_FLOAT32},
    {"float64", TOKEN_FLOAT64},
    {"bool", TOKEN_BOOL},
    {"char", TOKEN_CHAR_TYPE},
    {"string", TOKEN_STRING_TYPE},
    {"void", TOKEN_VOID},
    
    // 布尔字面量
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    
    {NULL, TOKEN_IDENTIFIER}  // 结束标记
};

TokenType keyword_from_string(const char* str, int length) {
    if (!str || length <= 0) return TOKEN_IDENTIFIER;
    
    for (const KeywordEntry* entry = keyword_table; entry->keyword != NULL; entry++) {
        if (strlen(entry->keyword) == (size_t)length && 
            strncmp(entry->keyword, str, length) == 0) {
            return entry->type;
        }
    }
    
    return TOKEN_IDENTIFIER;
}

// 调试输出
void token_print(const Token* token) {
    if (!token) {
        printf("Token: NULL\n");
        return;
    }
    
    printf("Token{type: %s, line: %d, col: %d, ", 
           token_type_to_string(token->type), token->line, token->column);
    
    if (token->start) {
        printf("text: '");
        for (int i = 0; i < token->length; i++) {
            putchar(token->start[i]);
        }
        printf("'");
        
        // 显示字面量值
        switch (token->type) {
            case TOKEN_INTEGER:
                printf(", value: %ld", token->int_value);
                break;
            case TOKEN_FLOAT:
                printf(", value: %g", token->float_value);
                break;
            case TOKEN_CHAR:
                printf(", value: '%c'", token->char_value);
                break;
            case TOKEN_STRING:
                if (token->string_value) {
                    printf(", value: \"%s\"", token->string_value);
                }
                break;
            case TOKEN_ERROR:
                if (token->error_message) {
                    printf(", error: %s", token->error_message);
                }
                break;
            default:
                break;
        }
    }
    
    printf("}\n");
}

char* token_to_string(const Token* token) {
    if (!token) return strdup("NULL");
    
    char buffer[256];
    int pos = 0;
    
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, 
                   "Token{type: %s, line: %d, col: %d", 
                   token_type_to_string(token->type), token->line, token->column);
    
    if (token->start && token->length > 0) {
        pos += snprintf(buffer + pos, sizeof(buffer) - pos, ", text: '");
        int text_len = token->length < 20 ? token->length : 20;
        for (int i = 0; i < text_len; i++) {
            if (pos < (int)sizeof(buffer) - 2) {
                buffer[pos++] = token->start[i];
            }
        }
        if (token->length > 20) {
            pos += snprintf(buffer + pos, sizeof(buffer) - pos, "...");
        }
        if (pos < (int)sizeof(buffer) - 2) {
            buffer[pos++] = '\'';
        }
    }
    
    if (pos < (int)sizeof(buffer) - 2) {
        buffer[pos++] = '}';
        buffer[pos] = '\0';
    }
    
    return strdup(buffer);
}