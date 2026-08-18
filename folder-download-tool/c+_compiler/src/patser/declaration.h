#ifndef DECLARATION_H
#define DECLARATION_H

#include "parser.h"

// 声明解析函数
Node* parse_declaration(Parser* parser);
Node* parse_function_declaration(Parser* parser);
Node* parse_variable_declaration(Parser* parser);
Node* parse_struct_declaration(Parser* parser);
Node* parse_enum_declaration(Parser* parser);
Node* parse_impl_declaration(Parser* parser);

// 工具函数
Vector* parse_parameter_list(Parser* parser);
Vector* parse_generic_parameters(Parser* parser);

#endif