#include "pattern.h"
#include "../ast/ast_utils.h"
#include <stdlib.h>

// 解析模式
Node* parse_pattern(Parser* parser) {
    return parse_pattern_primary(parser);
}

// 解析基础模式
Node* parse_pattern_primary(Parser* parser) {
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        char* name = strndup(parser->previous.start, parser->previous.length);
        
        // 检查是否是通配符模式
        if (strcmp(name, "_") == 0) {
            free(name);
            return ast_pattern_wildcard(parser->previous.line, parser->previous.column);
        }
        
        // 检查是否有子模式（解构）
        Node* subpattern = NULL;
        if (parser_match(parser, TOKEN_LBRACE)) {
            // 结构体模式
            Vector* field_patterns = vector_create(free_direct);
            
            while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
                Token field_name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name");
                parser_consume(parser, TOKEN_COLON, "Expected ':' after field name");
                
                Node* field_pattern = parse_pattern(parser);
                
                char* field_name_str = strndup(field_name.start, field_name.length);
                // 这里需要创建字段模式节点
                // 简化处理：直接使用标识符模式
                Node* field = ast_pattern_ident(field_name_str, false, field_pattern,
                                              field_name.line, field_name.column);
                vector_push(field_patterns, field);
                
                if (!parser_match(parser, TOKEN_COMMA)) {
                    break;
                }
            }
            
            parser_consume(parser, TOKEN_RBRACE, "Expected '}' after struct pattern");
            subpattern = ast_pattern_struct(name, field_patterns, parser->previous.line, parser->previous.column);
        } else if (parser_match(parser, TOKEN_LPAREN)) {
            // 元组模式
            Vector* element_patterns = vector_create(free_direct);
            
            if (!parser_check(parser, TOKEN_RPAREN)) {
                do {
                    Node* element_pattern = parse_pattern(parser);
                    vector_push(element_patterns, element_pattern);
                } while (parser_match(parser, TOKEN_COMMA));
            }
            
            parser_consume(parser, TOKEN_RPAREN, "Expected ')' after tuple pattern");
            subpattern = ast_pattern_tuple(element_patterns, parser->previous.line, parser->previous.column);
        }
        
        return ast_pattern_ident(name, false, subpattern, parser->previous.line, parser->previous.column);
    }
    
    if (parser_match(parser, TOKEN_MUT)) {
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name after 'mut'");
        char* var_name = strndup(name.start, name.length);
        return ast_pattern_ident(var_name, true, NULL, name.line, name.column);
    }
    
    // 字面量模式
    if (parser->current.type == TOKEN_INTEGER ||
        parser->current.type == TOKEN_FLOAT ||
        parser->current.type == TOKEN_STRING ||
        parser->current.type == TOKEN_CHAR ||
        parser->current.type == TOKEN_TRUE ||
        parser->current.type == TOKEN_FALSE) {
        
        parser_advance(parser);
        Node* literal = ast_literal_expr(parser->previous, parser->previous.line, parser->previous.column);
        return ast_pattern_literal(literal, parser->previous.line, parser->previous.column);
    }
    
    parser_error(parser, "Expected pattern");
    return NULL;
}