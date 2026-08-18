#include "lexer.h"
#include "../utils/string_builder.h"
#include <stdio.h>
#include <stdlib.h>

// 添加错误
void lexer_add_error(Lexer* lexer, const char* message, int line, int column) {
    lexer->had_error = true;
    
    StringBuilder* sb = sb_create();
    sb_append_format(sb, "[Line %d, Column %d] Lexer Error: %s", line, column, message);
    
    char* error_msg = sb_to_string(sb);
    vector_push(lexer->errors, error_msg);
    
    sb_free(sb);
}

// 检查是否有错误
bool lexer_has_errors(const Lexer* lexer) {
    return lexer && lexer->had_error;
}

// 打印所有错误
void lexer_print_errors(const Lexer* lexer) {
    if (!lexer || vector_is_empty(lexer->errors)) {
        return;
    }
    
    printf("Lexer Errors:\n");
    for (size_t i = 0; i < vector_size(lexer->errors); i++) {
        char* error = vector_get(lexer->errors, i);
        printf("  %s\n", error);
    }
}

// 获取当前扫描位置信息
void lexer_get_position(const Lexer* lexer, int* line, int* column) {
    if (line) *line = lexer->line;
    if (column) *column = lexer->column;
}