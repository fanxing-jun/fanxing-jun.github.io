#include "expression.h"
#include "../ast/ast_utils.h"
#include <stdlib.h>

// 表达式优先级
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_CONDITIONAL, // ?:
    PREC_OR,          // ||
    PREC_AND,         // &&
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * / %
    PREC_UNARY,       // ! - +
    PREC_CALL,        // . () []
    PREC_PRIMARY
} Precedence;

// 获取运算符优先级
static Precedence get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_EQ:
        case TOKEN_PLUS_EQ:
        case TOKEN_MINUS_EQ:
        case TOKEN_STAR_EQ:
        case TOKEN_SLASH_EQ:
        case TOKEN_PERCENT_EQ:
            return PREC_ASSIGNMENT;
            
        case TOKEN_QUESTION:
            return PREC_CONDITIONAL;
            
        case TOKEN_OR:
            return PREC_OR;
            
        case TOKEN_AND:
            return PREC_AND;
            
        case TOKEN_EQEQ:
        case TOKEN_NEQ:
            return PREC_EQUALITY;
            
        case TOKEN_LT:
        case TOKEN_GT:
        case TOKEN_LTEQ:
        case TOKEN_GTEQ:
            return PREC_COMPARISON;
            
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return PREC_TERM;
            
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:
            return PREC_FACTOR;
            
        case TOKEN_NOT:
        case TOKEN_PLUS:  // 一元正号
        case TOKEN_MINUS: // 一元负号
            return PREC_UNARY;
            
        case TOKEN_DOT:
        case TOKEN_LPAREN:
        case TOKEN_LBRACKET:
            return PREC_CALL;
            
        default:
            return PREC_NONE;
    }
}

// 通用表达式解析（Pratt解析器）
static Node* parse_precedence(Parser* parser, Precedence precedence) {
    parser_advance(parser);
    
    // 解析前缀表达式
    Node* left;
    switch (parser->previous.type) {
        case TOKEN_IDENTIFIER:
            left = ast_identifier_expr(
                strndup(parser->previous.start, parser->previous.length),
                parser->previous.line, parser->previous.column
            );
            break;
            
        case TOKEN_INTEGER:
        case TOKEN_FLOAT:
        case TOKEN_STRING:
        case TOKEN_CHAR:
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            left = parse_literal(parser);
            break;
            
        case TOKEN_LPAREN:
            left = parse_grouped_expression(parser);
            break;
            
        case TOKEN_LBRACKET:
            left = parse_array_expression(parser);
            break;
            
        case TOKEN_IF:
            left = parse_if_expression(parser);
            break;
            
        case TOKEN_MATCH:
            left = parse_match_expression(parser);
            break;
            
        case TOKEN_PIPE: // 闭包表达式开始
            left = parse_closure_expression(parser);
            break;
            
        case TOKEN_NOT:
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            {
                TokenType operator = parser->previous.type;
                Node* operand = parse_precedence(parser, PREC_UNARY);
                left = ast_unary_expr(operator, operand, 
                                    parser->previous.line, parser->previous.column);
            }
            break;
            
        case TOKEN_ASYNC:
            {
                parser_advance(parser);
                if (parser_match(parser, TOKEN_PIPE)) {
                    left = parse_closure_expression(parser);
                    // 标记为异步闭包
                    if (left && left->type == NODE_CLOSURE_EXPR) {
                        left->closure_expr.is_async = true;
                    }
                } else {
                    parser_error(parser, "Expected '|' after 'async'");
                    left = NULL;
                }
            }
            break;
            
        case TOKEN_AWAIT:
            {
                Node* expression = parse_precedence(parser, PREC_UNARY);
                left = ast_await_expr(expression, parser->previous.line, parser->previous.column);
            }
            break;
            
        default:
            parser_error(parser, "Expected expression");
            left = NULL;
            break;
    }
    
    if (!left) return NULL;
    
    // 解析中缀表达式
    while (precedence <= get_precedence(parser->current.type)) {
        parser_advance(parser);
        
        switch (parser->previous.type) {
            case TOKEN_DOT:
                left = parse_field_access(parser, left);
                break;
                
            case TOKEN_LPAREN:
                left = parse_call_expr(parser, left);
                break;
                
            case TOKEN_LBRACKET:
                left = parse_array_access(parser, left);
                break;
                
            default:
                {
                    TokenType operator = parser->previous.type;
                    Node* right = parse_precedence(parser, get_precedence(operator));
                    left = ast_binary_expr(operator, left, right,
                                         parser->previous.line, parser->previous.column);
                }
                break;
        }
        
        if (!left) return NULL;
    }
    
    return left;
}

// 主表达式解析入口
Node* parse_expression(Parser* parser) {
    return parse_precedence(parser, PREC_ASSIGNMENT);
}

// 解析字面量
Node* parse_literal(Parser* parser) {
    Token token = parser->previous;
    Node* node = ast_literal_expr(token, token.line, token.column);
    
    // 设置字面量值
    if (node && node->type == NODE_LITERAL_EXPR) {
        // 这里可以设置具体的字面量值
        // 例如：node->literal_expr.value = ...;
    }
    
    return node;
}

// 解析分组表达式
Node* parse_grouped_expression(Parser* parser) {
    Node* expression = parse_expression(parser);
    parser_consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
    return ast_group_expr(expression, parser->previous.line, parser->previous.column);
}

// 解析字段访问
Node* parse_field_access(Parser* parser, Node* object) {
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name after '.'");
    char* field_name = strndup(name.start, name.length);
    return ast_field_access_expr(object, field_name, name.line, name.column);
}

// 解析函数调用
Node* parse_call_expr(Parser* parser, Node* callee) {
    Vector* arguments = NULL;
    
    if (!parser_check(parser, TOKEN_RPAREN)) {
        arguments = vector_create(free_direct);
        do {
            if (vector_size(arguments) >= 255) {
                parser_error(parser, "Cannot have more than 255 arguments");
            }
            
            Node* arg = parse_expression(parser);
            if (arg) {
                vector_push(arguments, arg);
            }
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_RPAREN, "Expected ')' after arguments");
    return ast_call_expr(callee, arguments, parser->previous.line, parser->previous.column);
}

// 解析数组访问
Node* parse_array_access(Parser* parser, Node* array) {
    Node* index = parse_expression(parser);
    parser_consume(parser, TOKEN_RBRACKET, "Expected ']' after index");
    return ast_array_access_expr(array, index, parser->previous.line, parser->previous.column);
}

// 解析if表达式
Node* parse_if_expression(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Node* condition = parse_expression(parser);
    parser_consume(parser, TOKEN_LBRACE, "Expected '{' after if condition");
    
    Node* then_branch = parse_block_statement(parser);
    
    Node* else_branch = NULL;
    if (parser_match(parser, TOKEN_ELSE)) {
        if (parser_match(parser, TOKEN_IF)) {
            else_branch = parse_if_expression(parser);
        } else {
            parser_consume(parser, TOKEN_LBRACE, "Expected '{' after else");
            else_branch = parse_block_statement(parser);
        }
    }
    
    return ast_if_expr(condition, then_branch, else_branch, line, column);
}

// 解析数组表达式
Node* parse_array_expression(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Vector* elements = vector_create(free_direct);
    
    if (!parser_check(parser, TOKEN_RBRACKET)) {
        do {
            Node* element = parse_expression(parser);
            if (element) {
                vector_push(elements, element);
            }
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_RBRACKET, "Expected ']' after array elements");
    return ast_array_expr(elements, line, column);
}

// 解析闭包表达式
Node* parse_closure_expression(Parser* parser) {
    int line = parser->previous.line;
    int column = parser->previous.column;
    
    Vector* params = vector_create(free_direct);
    
    // 解析参数
    if (!parser_check(parser, TOKEN_PIPE)) {
        do {
            if (parser_match(parser, TOKEN_MUT)) {
                // 可变参数
                Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
                char* param_name = strndup(name.start, name.length);
                Node* param = ast_param(param_name, NULL, true, name.line, name.column);
                vector_push(params, param);
            } else if (parser_check(parser, TOKEN_IDENTIFIER)) {
                Token name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
                char* param_name = strndup(name.start, name.length);
                Node* param = ast_param(param_name, NULL, false, name.line, name.column);
                vector_push(params, param);
            } else {
                parser_error(parser, "Expected parameter in closure");
                break;
            }
        } while (parser_match(parser, TOKEN_COMMA));
    }
    
    parser_consume(parser, TOKEN_PIPE, "Expected '|' after closure parameters");
    
    // 解析返回类型（可选）
    Node* return_type = NULL;
    if (parser_match(parser, TOKEN_ARROW)) {
        return_type = parse_type_annotation(parser);
    }
    
    // 解析函数体
    Node* body;
    if (parser_match(parser, TOKEN_LBRACE)) {
        body = parse_block_statement(parser);
    } else {
        // 单表达式闭包
        Node* expr = parse_expression(parser);
        Vector* stmts = vector_create(free_direct);
        vector_push(stmts, ast_expr_stmt(expr, expr->line, expr->column));
        body = ast_block_stmt(stmts, expr->line, expr->column);
    }
    
    return ast_closure_expr(params, return_type, body, false, line, column);
}