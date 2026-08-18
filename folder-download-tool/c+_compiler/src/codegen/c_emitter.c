#include "c_emitter.h"
#include "../utils/string_builder.h"
#include <stdlib.h>

// 发射函数序言
void emitter_emit_function_prologue(CodeGenerator* cg, Node* function_node) {
    if (!cg || !function_node || function_node->type != NODE_FUNCTION_DECL) return;
    
    // 生成函数签名
    const char* return_type = codegen_type_to_c_type(function_node->func_decl.return_type->type_annotation.type);
    codegen_emit(cg, "%s %s(", return_type, function_node->func_decl.name);
    
    // 生成参数列表
    for (size_t i = 0; i < vector_size(function_node->func_decl.params); i++) {
        Node* param = vector_get(function_node->func_decl.params, i);
        if (param->type != NODE_PARAM) continue;
        
        const char* param_type = codegen_type_to_c_type(param->param.type_annotation->type_annotation.type);
        codegen_emit(cg, "%s %s", param_type, param->param.name);
        
        if (i < vector_size(function_node->func_decl.params) - 1) {
            codegen_emit(cg, ", ");
        }
    }
    
    codegen_emit_line(cg, ") {");
    codegen_indent_increase(cg);
}

// 发射函数结尾
void emitter_emit_function_epilogue(CodeGenerator* cg, Node* function_node) {
    if (!cg) return;
    
    // 如果函数返回void且没有return语句，添加默认return
    Type* return_type = function_node->func_decl.return_type->type_annotation.type;
    if (return_type->kind == TYPE_VOID) {
        codegen_emit_line(cg, "return;");
    }
    
    codegen_indent_decrease(cg);
    codegen_emit_line(cg, "}");
    codegen_emit_line(cg, "");
}

// 发射变量声明
void emitter_emit_variable_declaration(CodeGenerator* cg, const char* name, Type* type, const char* initializer) {
    if (!cg || !name || !type) return;
    
    const char* c_type = codegen_type_to_c_type(type);
    codegen_indent(cg);
    
    if (initializer) {
        codegen_emit_line(cg, "%s %s = %s;", c_type, name, initializer);
    } else {
        codegen_emit_line(cg, "%s %s;", c_type, name);
    }
}

// 发射结构体定义
void emitter_emit_struct_definition(CodeGenerator* cg, Node* struct_node) {
    if (!cg || !struct_node || struct_node->type != NODE_STRUCT_DECL) return;
    
    codegen_emit_line(cg, "typedef struct {");
    codegen_indent_increase(cg);
    
    // 发射字段
    for (size_t i = 0; i < vector_size(struct_node->struct_decl.fields); i++) {
        Node* field = vector_get(struct_node->struct_decl.fields, i);
        if (field->type != NODE_FIELD_DEF) continue;
        
        const char* field_type = codegen_type_to_c_type(field->field_def.type_annotation->type_annotation.type);
        codegen_emit_line(cg, "%s %s;", field_type, field->field_def.name);
    }
    
    codegen_indent_decrease(cg);
    codegen_emit_line(cg, "} %s;", struct_node->struct_decl.name);
    codegen_emit_line(cg, "");
}