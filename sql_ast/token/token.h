#pragma once

#include <map>
#include <string>
using std::string;

#include <json/json.h>
using namespace luo::json;

namespace dt
{
    namespace token
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
                TOKEN_COMMA,           // ,
                TOKEN_SEMICOLON,       // ;

                TOKEN_IDENTIFIER,      // identifier 标识符
                TOKEN_NULL,            // null

                TOKEN_DATABASE,        // database
                TOKEN_TABLE,           // table
                TOKEN_ENGINE,          // engine
                TOKEN_USE,             // use
                TOKEN_CREATE,          // create
                TOKEN_DELETE,          // delete
                TOKEN_UPDATE,          // update
                TOKEN_SELECT,          // select

                TOKEN_TTL,             // ttl
                TOKEN_FROM,            // from
                TOKEN_WHERE,           // where
                TOKEN_TIME,            // time

            };

            Token();
            Token(Type type, const string & literal);
            ~Token() = default;

            Type type() const;
            string name() const;
            string literal() const;

            Token & operator = (const Token & other);
            static Type look_up(const string & literal);

            void show() const;
            Json json() const;

        private:
            Type                                m_type;      // 符号类型
            string                              m_literal;   // 字面量，也就是完整的串
            static std::map<Type, string>       m_names;     // 关系表
            static std::map<string, Type>       m_keywords;  // 关键字反查表
        };
    }
}