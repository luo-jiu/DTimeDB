#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;
using std::string;

#include <chrono>
using namespace std::chrono;

std::shared_ptr<Expression> Parser::parse_insert()
{
    // insert into school,class='高一2班' '温度'=21, '湿度'=72.1 1702880186879347823
    std::shared_ptr<Insert> e = std::make_shared<Insert>();
    e->m_token = m_curr;
    next_token();  // 跳过insert 关键字
    if (m_curr.type() == Token::TOKEN_INTO && peek_token_is(Token::TOKEN_STRING))  // into
    {
        next_token();
        if (peek_token_is(Token::TOKEN_DOT))  // 若是'.'
        {
            e->m_curr_db_name = m_curr.literal();
            next_token();
            if (!peek_token_is(Token::TOKEN_STRING))
            {
                return nullptr;
            }
            next_token();
        }
        e->m_from = m_curr.literal();
        next_token();

        if(m_curr.type() == Token::TOKEN_COMMA)  // token 是否为','
        {
            next_token();  // 跳过','
            string tag;
            while (peek_token_is(Token::TOKEN_EQ))  // 下一个token 为'='
            {
                tag += m_curr.literal();
                next_token();
                next_token();
                tag += ("=" + m_curr.literal());
                next_token();

                e->m_tags.push_back(tag);
                tag = "";
                if (m_curr.type() == Token::TOKEN_COMMA)
                {
                    next_token();
                }
                else
                {
                    break;
                }
            }
        }
        while (peek_token_is(Token::TOKEN_EQ))
        {
            e->m_fields.push_back(m_curr.literal());
            next_token();
            next_token();
            switch (m_curr.type())
            {
                case Token::TOKEN_STRING:
                {
                    e->m_values.push_back(m_curr.literal());
                    break;
                }
                case Token::TOKEN_INTEGER:
                {
                    e->m_values.push_back(m_curr.literal());
                    break;
                }
                case Token::TOKEN_FLOAT:
                {
                    e->m_values.push_back(m_curr.literal());
                    break;
                }
                default:
                {
                    std::cerr << "Error : Unknown type" << std::to_string(m_curr.type()) << std::endl;
                }
            }
            next_token();

            if (m_curr.type() == Token::TOKEN_COMMA)
            {
                next_token();
            }
            else
            {
                break;
            }
        }

        if (!peek_token_is(Token::TOKEN_EOF))
        {
            e->m_timestamp = m_curr.literal();
        }
        else
        {
            // 此时生成新的timestamp
            auto timestamp = high_resolution_clock::now();
            auto duration = timestamp.time_since_epoch();
            auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
            string timestamp_str = std::to_string(nanoseconds.count());
            e->m_timestamp = timestamp_str;
        }

        return e;
    }
    else
    {
        std::ostringstream oss;
        oss << "Error : insert.cpp '"<< m_curr.literal() <<"' Syntax error";
        m_errors.push_back(oss.str());
    }
}