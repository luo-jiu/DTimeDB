#pragma once

#include "json/json.h"
using namespace dt::json;

#include <map>
#include <string>

namespace dt::token
{
    /**
     * 符号定义
     */
    class Token
    {
    public:
        enum Type
        {
            TOKEN_ILLEGAL = 0,     // illegal
            TOKEN_EOF,             // eof
            TOKEN_INTEGER,         // integer
            TOKEN_FLOAT,           // float
            TOKEN_STRING,          // string
            TOKEN_PLUS,            // +
            TOKEN_MINUS,           // -
            TOKEN_ASTERISK,        // *
            TOKEN_SLASH,           // /
            TOKEN_MODULO,          // %
            TOKEN_DECREASE,        // --
            TOKEN_AND,             // AND
            TOKEN_OR,              // OR
            TOKEN_LT,              // <
            TOKEN_GT,              // >
            TOKEN_LE,              // <=
            TOKEN_GE,              // >=
            TOKEN_EQ,              // =
            TOKEN_NE,              // !=
            TOKEN_LPAREN,          // (
            TOKEN_RPAREN,          // )
            TOKEN_DOT,             // .
            TOKEN_COMMA,           // ,
            TOKEN_SEMICOLON,       // ;
            TOKEN_TIMESTAMP,       // timestamp

            TOKEN_IDENTIFIER,      // identifier 标识符
            TOKEN_NULL,            // null

            TOKEN_DATABASE,        // database
            TOKEN_DATABASES,       // databases
            TOKEN_TABLE,           // table
            TOKEN_TABLES,          // tables
            TOKEN_ENGINE,          // engine
            TOKEN_SHOW,            // show
            TOKEN_USE,             // use
            TOKEN_CREATE,          // create

            TOKEN_INSERT,          // insert.cpp
            TOKEN_INTO,            // into
            TOKEN_UPDATE,          // update
            TOKEN_SELECT,          // select

            TOKEN_TTL,             // ttl
            TOKEN_FROM,            // m_from
            TOKEN_WHERE,           // m_where

            TOKEN_TSM,             // tsm
            TOKEN_CLT,             // clt

            TOKEN_REDUCE,          // reduce
            TOKEN_SYSTEM,          // system command
        };

        Token();
        Token(Type type, const std::string & literal);
        ~Token() = default;

        Type type() const;
        std::string name() const;
        std::string literal() const;

        Token & operator = (const Token & other);
        static Type look_up(const std::string & literal);

        void show() const;
        Json json() const;

    private:
        Type                                     m_type;      // 符号类型
        std::string                              m_literal;   // 字面量，也就是完整的串
        static std::map<Type, std::string>       m_names;     // 关系表
        static std::map<std::string, Type>       m_keywords;  // 关键字反查表
    };
}
