#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/symbol_table.h"
#include "semantic/type_checker.h"
#include "codegen/codegen.h"

void compile_file(const char* input_file, const char* output_file) {
    printf("Compiling %s -> %s\n", input_file, output_file);
    
    // 读取源文件
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s\n", input_file);
        exit(1);
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, fp);
    source[size] = '\0';
    fclose(fp);
    
    // 词法分析
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        exit(1);
    }
    
    // 语法分析
    Parser* parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_free(lexer);
        free(source);
        exit(1);
    }
    
    Node* ast = parser_parse(parser);
    if (!ast) {
        fprintf(stderr, "Error: Failed to parse\n");
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        exit(1);
    }
    
    if (parser_has_errors(parser)) {
        printf("Parser errors:\n");
        parser_print_errors(parser);
        // 继续执行，看是否能生成代码
    }
    
    // 语义分析
    SymbolTable* symtab = symbol_table_create();
    TypeChecker* tc = type_checker_create(symtab);
    
    if (!type_check_program(tc, ast)) {
        printf("Type checker errors:\n");
        type_checker_print_errors(tc);
        // 继续执行，看是否能生成代码
    }
    
    // 代码生成
    FILE* output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "Error: Cannot open output file %s\n", output_file);
        type_checker_free(tc);
        symbol_table_free(symtab);
        ast_free(ast);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        exit(1);
    }
    
    CodeGenerator* cg = codegen_create(output, symtab);
    if (!codegen_generate(cg, ast)) {
        printf("Code generation errors:\n");
        codegen_print_errors(cg);
    }
    
    fclose(output);
    
    // 清理
    codegen_free(cg);
    type_checker_free(tc);
    symbol_table_free(symtab);
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    printf("Compilation completed!\n");
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: c+ <input.cplus> <output.c>\n");
        printf("Example: c+ hello.cplus hello.c\n");
        return 1;
    }
    
    compile_file(argv[1], argv[2]);
    return 0;
}