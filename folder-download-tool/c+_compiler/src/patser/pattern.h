#ifndef PATTERN_H
#define PATTERN_H

#include "parser.h"

// 模式解析函数
Node* parse_pattern(Parser* parser);
Node* parse_pattern_primary(Parser* parser);

#endif