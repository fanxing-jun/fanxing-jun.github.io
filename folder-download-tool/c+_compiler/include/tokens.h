#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>

typedef enum {
    // ===== 字面量 =====
    TOKEN_EOF,              // 文件结束
    TOKEN_IDENTIFIER,       // 标识符
    TOKEN_INTEGER,          // 整数字面量
    TOKEN_FLOAT,            // 浮点数字面量
    TOKEN_STRING,           // 字符串字面量
    TOKEN_CHAR,             // 字符字面量
    
    // ===== 关键字 =====
    TOKEN_FUNC,             // func
    TOKEN_LET,              // let
    TOKEN_MUT,              // mut
    TOKEN_CONST,            // const
    TOKEN_IF,               // if
    TOKEN_ELSE,             // else
    TOKEN_FOR,              // for
    TOKEN_WHILE,            // while
    TOKEN_LOOP,             // loop
    TOKEN_MATCH,            // match
    TOKEN_RETURN,           // return
    TOKEN_BREAK,            // break
    TOKEN_CONTINUE,         // continue
    TOKEN_STRUCT,           // struct
    TOKEN_ENUM,             // enum
    TOKEN_IMPL,             // impl
    TOKEN_INTERFACE,        // interface
    TOKEN_TRAIT,            // trait
    TOKEN_MODULE,           // module
    TOKEN_IMPORT,           // import
    TOKEN_AS,               // as
    TOKEN_PUB,              // pub
    TOKEN_PRIV,             // priv
    TOKEN_ASYNC,            // async
    TOKEN_AWAIT,            // await
    TOKEN_SPAWN,            // spawn
    TOKEN_TYPE,             // type
    TOKEN_SELF,             // self
    TOKEN_SUPER,            // super
    
    // ===== 类型关键字 =====
    TOKEN_INT,              // int
    TOKEN_INT8,             // int8
    TOKEN_INT16,            // int16
    TOKEN_INT32,            // int32
    TOKEN_INT64,            // int64
    TOKEN_UINT,             // uint
    TOKEN_UINT8,            // uint8
    TOKEN_UINT16,           // uint16
    TOKEN_UINT32,           // uint32
    TOKEN_UINT64,           // uint64
    TOKEN_FLOAT32,          // float32
    TOKEN_FLOAT64,          // float64
    TOKEN_BOOL,             // bool
    TOKEN_CHAR_TYPE,        // char
    TOKEN_STRING_TYPE,      // string
    TOKEN_VOID,             // void
    
    // ===== 布尔字面量 =====
    TOKEN_TRUE,             // true
    TOKEN_FALSE,            // false
    
    // ===== 运算符 =====
    // 算术运算符
    TOKEN_PLUS,             // +
    TOKEN_MINUS,            // -
    TOKEN_STAR,             // *
    TOKEN_SLASH,            // /
    TOKEN_PERCENT,          // %
    
    // 赋值运算符
    TOKEN_EQ,               // =
    TOKEN_PLUS_EQ,          // +=
    TOKEN_MINUS_EQ,         // -=
    TOKEN_STAR_EQ,          // *=
    TOKEN_SLASH_EQ,         // /=
    TOKEN_PERCENT_EQ,       // %=
    
    // 比较运算符
    TOKEN_EQEQ,             // ==
    TOKEN_NEQ,              // !=
    TOKEN_LT,               // <
    TOKEN_GT,               // >
    TOKEN_LTEQ,             // <=
    TOKEN_GTEQ,             // >=
    
    // 逻辑运算符
    TOKEN_AND,              // &&
    TOKEN_OR,               // ||
    TOKEN_NOT,              // !
    
    // 位运算符
    TOKEN_AMP,              // &
    TOKEN_PIPE,             // |
    TOKEN_CARET,            // ^
    TOKEN_TILDE,            // ~
    TOKEN_LSHIFT,           // <<
    TOKEN_RSHIFT,           // >>
    
    // 其他运算符
    TOKEN_QUESTION,         // ?
    TOKEN_DOT,              // .
    TOKEN_DOTDOT,           // ..
    TOKEN_DOTDOTDOT,        // ...
    TOKEN_ARROW,            // ->
    TOKEN_FAT_ARROW,        // =>
    TOKEN_AT,               // @
    
    // ===== 标点符号 =====
    TOKEN_LPAREN,           // (
    TOKEN_RPAREN,           // )
    TOKEN_LBRACE,           // {
    TOKEN_RBRACE,           // }
    TOKEN_LBRACKET,         // [
    TOKEN_RBRACKET,         // ]
    TOKEN_COMMA,            // ,
    TOKEN_COLON,            // :
    TOKEN_SEMICOLON,        // ;
    TOKEN_DOUBLE_COLON,     // ::
    
    // ===== 特殊 =====
    TOKEN_ERROR,            // 词法错误
    TOKEN_COMMENT,          // 注释
    TOKEN_LINE_COMMENT,     // 行注释
    
    // ===== 宏相关 =====
    TOKEN_HASH,             // #
    TOKEN_DOLLAR,           // $
    
    // ===== 所有权相关 =====
    TOKEN_AMP_MUT,          // &mut
    TOKEN_AMP_OWNED,        // &owned
    
    _TOKEN_TYPE_COUNT       // 用于统计类型数量
} TokenType;

// Token 结构体
typedef struct {
    TokenType type;         // Token 类型
    const char* start;      // Token 在源文件中的起始位置
    int length;             // Token 长度
    int line;               // 所在行号
    int column;             // 所在列号
    
    // 字面量值（根据类型使用不同的联合成员）
    union {
        long int_value;         // 整数值
        double float_value;     // 浮点数值
        char char_value;        // 字符值
        char* string_value;     // 字符串值（需要复制）
        char* error_message;    // 错误信息
    };
} Token;

#endif