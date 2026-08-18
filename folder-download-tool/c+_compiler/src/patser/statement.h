#ifndef STATEMENT_H
#define STATEMENT_H

#include "parser.h"

// 语句解析函数
Node* parse_statement(Parser* parser);
Node* parse_block_statement(Parser* parser);
Node* parse_expression_statement(Parser* parser);
Node* parse_if_statement(Parser* parser);
Node* parse_for_statement(Parser* parser);
Node* parse_while_statement(Parser* parser);
Node* parse_loop_statement(Parser* parser);
Node* parse_match_statement(Parser* parser);
Node* parse_return_statement(Parser* parser);
Node* parse_break_statement(Parser* parser);
Node* parse_continue_statement(Parser* parser);

// 工具函数
Node* parse_type_annotation(Parser* parser);

#endif