#include "statement.h"
#include "expression.h"
#include "pattern.h"
#include "../ast/ast_utils.h"
#include <stdlib.h>

// 解析语句
Node* parse_statement(Parser* parser) {
    if (parser_match(parser, TOKEN_LBRACE)) {
        return parse_block_statement(parser);
    }
    if (parser_match(parser, TOKEN_IF)) {
        return parse_if_statement(parser);
    }
    if (parser_match(parser, TOKEN_FOR)) {
        return parse_for_statement(parser);
    }
    if (parser_match(parser, TOKEN_WHILE)) {
        return parse_while_statement(parser);
    }
    if (parser_match(parser, TOKEN_LOOP)) {
        return parse_loop_statement(parser);
    }
    if (parser_match(parser, TOKEN_MATCH)) {
        return parse_match_statement(parser);
    }
    if (parser_match(parser, TOKEN_RETURN)) {
        return parse_return_statement(parser);
    }
    if (parser_match(parser, TOKEN_BREAK)) {
        return parse_break_statement(parser);
    }
    if (parser_match(parser, TOKEN_CONTINUE)) {
        return parse_continue_statement(parser);
    }
    
    return parse_expression_statement(parser);
}

// 解析块语句
Node* parse_block_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Vector* statements = vector_create(free_direct);
    
    while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        Node* stmt = parse_statement(parser);
        if (stmt) {
            vector_push(statements, stmt);
        }
        
        if (parser->panic_mode) {
            parser_synchronize(parser);
        }
    }
    
    parser_consume(parser, TOKEN_RBRACE, "Expected '}' after block");
    return ast_block_stmt(statements, line, column);
}

// 解析表达式语句
Node* parse_expression_statement(Parser* parser) {
    Node* expr = parse_expression(parser);
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression");
    return ast_expr_stmt(expr, expr->line, expr->column);
}

// 解析if语句
Node* parse_if_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* condition = parse_expression(parser);
    Node* then_branch = parse_statement(parser);
    
    Node* else_branch = NULL;
    if (parser_match(parser, TOKEN_ELSE)) {
        else_branch = parse_statement(parser);
    }
    
    return ast_if_stmt(condition, then_branch, else_branch, line, column);
}

// 解析for循环
Node* parse_for_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    // 解析模式（let x 或 mut x）
    Node* pattern;
    if (parser_match(parser, TOKEN_LET)) {
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name in for loop");
        char* var_name = strndup(name.start, name.length);
        pattern = ast_pattern_ident(var_name, false, NULL, name.line, name.column);
    } else if (parser_match(parser, TOKEN_MUT)) {
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name in for loop");
        char* var_name = strndup(name.start, name.length);
        pattern = ast_pattern_ident(var_name, true, NULL, name.line, name.column);
    } else {
        parser_error(parser, "Expected 'let' or 'mut' in for loop");
        pattern = NULL;
    }
    
    parser_consume(parser, TOKEN_IN, "Expected 'in' after for pattern");
    
    Node* iterable = parse_expression(parser);
    Node* body = parse_statement(parser);
    
    return ast_for_stmt(pattern, iterable, body, line, column);
}

// 解析while循环
Node* parse_while_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* condition = parse_expression(parser);
    Node* body = parse_statement(parser);
    
    return ast_while_stmt(condition, body, line, column);
}

// 解析loop循环
Node* parse_loop_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* body = parse_block_statement(parser);
    return ast_loop_stmt(body, line, column); // 需要添加ast_loop_stmt函数
}

// 解析match语句
Node* parse_match_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* subject = parse_expression(parser);
    parser_consume(parser, TOKEN_LBRACE, "Expected '{' after match subject");
    
    Vector* arms = vector_create(free_direct);
    
    while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        Node* pattern = parse_pattern(parser);
        Node* guard = NULL;
        
        if (parser_match(parser, TOKEN_IF)) {
            guard = parse_expression(parser);
        }
        
        parser_consume(parser, TOKEN_ARROW, "Expected '=>' after match pattern");
        
        Node* body;
        if (parser_match(parser, TOKEN_LBRACE)) {
            body = parse_block_statement(parser);
        } else {
            // 单表达式匹配臂
            Node* expr = parse_expression(parser);
            Vector* stmts = vector_create(free_direct);
            vector_push(stmts, ast_expr_stmt(expr, expr->line, expr->column));
            body = ast_block_stmt(stmts, expr->line, expr->column);
            parser_consume(parser, TOKEN_COMMA, "Expected ',' after match arm");
        }
        
        Node* arm = ast_match_arm(pattern, guard, body, pattern->line, pattern->column);
        vector_push(arms, arm);
    }
    
    parser_consume(parser, TOKEN_RBRACE, "Expected '}' after match arms");
    return ast_match_stmt(subject, arms, line, column);
}

// 解析return语句
Node* parse_return_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* value = NULL;
    if (!parser_check(parser, TOKEN_SEMICOLON)) {
        value = parse_expression(parser);
    }
    
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after return value");
    return ast_return_stmt(value, line, column);
}

// 解析break语句
Node* parse_break_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after break");
    return ast_break_stmt(line, column);
}

// 解析continue语句
Node* parse_continue_statement(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after continue");
    return ast_continue_stmt(line, column);
}

// 解析类型注解
Node* parse_type_annotation(Parser* parser) {
    // 简化版本：只解析基本类型
    if (parser_match(parser, TOKEN_INT) ||
        parser_match(parser, TOKEN_FLOAT32) ||
        parser_match(parser, TOKEN_FLOAT64) ||
        parser_match(parser, TOKEN_BOOL) ||
        parser_match(parser, TOKEN_STRING_TYPE) ||
        parser_match(parser, TOKEN_CHAR_TYPE) ||
        parser_match(parser, TOKEN_VOID)) {
        
        TypeKind kind;
        switch (parser->previous.type) {
            case TOKEN_INT: kind = TYPE_INT32; break;
            case TOKEN_FLOAT32: kind = TYPE_FLOAT32; break;
            case TOKEN_FLOAT64: kind = TYPE_FLOAT64; break;
            case TOKEN_BOOL: kind = TYPE_BOOL; break;
            case TOKEN_STRING_TYPE: kind = TYPE_STRING; break;
            case TOKEN_CHAR_TYPE: kind = TYPE_CHAR; break;
            case TOKEN_VOID: kind = TYPE_VOID; break;
            default: kind = TYPE_UNKNOWN; break;
        }
        
        Type* type = type_create_basic(kind, token_type_to_string(parser->previous.type));
        return ast_type_annotation(type, parser->previous.line, parser->previous.column);
    }
    
    parser_error(parser, "Expected type annotation");
    return NULL;
}