#pragma once

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <ast/header.h>
using namespace dt::ast;

namespace dt
{
    namespace parser
    {
        /**
         * 语法分析器
         */
        class Parser
        {
        public:
            enum Precedence  // 优先级定义
            {
                LOWEST = 0,  // 最低优先级
                LOGIC,       // AND OR
                COMPARE,     // < > <= >= = !=
                SUM,         // + -
                PRODUCT,     // * / %
                PREFIX,      // -a
            };

            Parser();
            Parser(const std::shared_ptr<Lexer> &lexer);
            ~Parser();

            // 前缀表达式构建函数原型定义
            typedef std::shared_ptr<Expression> (Parser::*prefix_parse_fn)(void);

            // 中缀表达式构建函数原型定义
            typedef std::shared_ptr<Expression> (Parser::*infix_parse_fn)(const std::shared_ptr<Expression> &);

            void next_token();

            // 判断是不是这个类型
            bool curr_token_is(Token::Type type);
            bool peek_token_is(Token::Type type);

            bool expect_peer_token(Token::Type type);  // 期待下一个token 类型

            // 记录错误
            void peek_error(Token::Type type);
            void no_prefix_parse_fn_error(Token::Type type);

            // 获取优先级(当前& 下一个)
            int curr_token_precedence();
            int peek_token_precedence();

            std::shared_ptr<Program> parse_program();
            std::shared_ptr<Statement> parse_statement();
            std::shared_ptr<ExpressionStatement> parse_expression_statement();

            // 前缀表达式
            std::shared_ptr<Expression> parse_integer();
            std::shared_ptr<Expression> parse_group();
            std::shared_ptr<Expression> parse_prefix();
            std::shared_ptr<Expression> parse_float();
            std::shared_ptr<Expression> parse_string();
            std::shared_ptr<Expression> parse_null();
            std::shared_ptr<Expression> parse_identifier();

            std::shared_ptr<Expression> parse_select();
            std::shared_ptr<Expression> parse_insert();

            // 中缀表达式
            std::shared_ptr<Expression> parse_infix(const std::shared_ptr<Expression> &left);

            std::shared_ptr<Expression> parse_expression(int precedence);  // 生成树
            std::list<string> & errors();  // 错误信息

        private:
            std::shared_ptr<Lexer>                  m_lexer;
            Token                                   m_curr;  // 当前符号
            Token                                   m_peek;  // 下一个符号
            std::list<string>                       m_errors;  // 错误信息
            static std::map<Token::Type, int>       m_precedence;  // 优先级关系

            static std::map<Token::Type, prefix_parse_fn>       m_prefix_parse_fns;  // 前缀表达式
            static std::map<Token::Type, infix_parse_fn>        m_infix_parse_fns;   // 中缀表达式
        };
    }
}