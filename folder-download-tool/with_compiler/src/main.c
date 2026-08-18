// src/main.c
#include "lexer/lexer.h"
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <source.with>\n", argv[0]);
        return 1;
    }
    
    // 读取源文件
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = (char*)malloc(file_size + 1);
    fread(source, 1, file_size, file);
    source[file_size] = '\0';
    fclose(file);
    
    // 初始化词法分析器
    Lexer* lexer = lexer_init(source);
    
    // 逐个读取并打印 Token
    Token* token;
    int token_count = 0;
    
    printf("=== Token Stream ===\n");
    do {
        token = lexer_next_token(lexer);
        token_print(token);
        token_count++;
        
        if (token->type == TOKEN_ERROR) {
            printf("\nError at line %d: Unexpected character '%s'\n", 
                   token->line, token->lexeme);
            token_free(token);
            break;
        }
        
        token_free(token);
    } while (token->type != TOKEN_EOF);
    
    printf("\nTotal tokens: %d\n", token_count);
    
    // 清理
    lexer_free(lexer);
    free(source);
    
    return 0;
}