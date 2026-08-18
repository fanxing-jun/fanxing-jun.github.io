#ifndef SCOPE_H
#define SCOPE_H

#include "symbol_table.h"

// 作用域管理专用函数
Scope* scope_create_with_function(Scope* parent, Symbol* function_symbol);
Scope* scope_create_with_struct(Scope* parent, Symbol* struct_symbol);
Scope* scope_create_with_loop(Scope* parent, bool can_break, bool can_continue);
Scope* scope_create_with_module(Scope* parent, const char* module_name);

// 作用域查询
bool scope_is_global(const Scope* scope);
bool scope_is_function(const Scope* scope);
bool scope_is_loop(const Scope* scope);
bool scope_is_struct(const Scope* scope);

// 符号查找
Symbol* scope_lookup(const Scope* scope, const char* name);
Symbol* scope_lookup_local(const Scope* scope, const char* name);
bool scope_define(Scope* scope, Symbol* symbol);

// 作用域遍历
typedef void (*ScopeVisitor)(Scope* scope, void* context);
void scope_traverse(Scope* scope, ScopeVisitor visitor, void* context);

#endif