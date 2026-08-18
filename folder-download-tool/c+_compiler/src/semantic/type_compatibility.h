#ifndef TYPE_COMPATIBILITY_H
#define TYPE_COMPATIBILITY_H

#include "type_checker.h"

// 类型兼容性检查
bool types_are_compatible(TypeChecker* tc, Type* actual, Type* expected);
bool types_are_assignable(TypeChecker* tc, Type* target, Type* source);
bool types_are_equal(Type* a, Type* b);

// 类型特征检查
bool is_numeric_type(Type* type);
bool is_integer_type(Type* type);
bool is_float_type(Type* type);
bool is_boolean_type(Type* type);
bool is_void_type(Type* type);

// 类型解析
Type* resolve_type_annotation(TypeChecker* tc, Node* type_annotation);

#endif