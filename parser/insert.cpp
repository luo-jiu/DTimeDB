#include <parser/parser.h>
using namespace dt::parser;

std::shared_ptr<Expression> Parser::parse_insert()
{
    // insert into from school,class='高一2班' '温度'=21, '湿度'=72.1 1702880186879347823
    std::shared_ptr<Insert> e = std::make_shared<Insert>();
    e->m_token = m_curr;
    next_token();  // 跳过insert 关键字
    if (m_curr.type() == Token::TOKEN_INTO && peek_token_is(Token::TOKEN_FROM))  // into from
    {
        next_token();  // 跳过into 关键字
        next_token();  // 跳过from 关键字
        e->m_from = parse_string();
        next_token();  // 跳过表名

        if(m_curr.type() == Token::TOKEN_COMMA)  // token 是否为','
        {
            next_token();  // 跳过','
            string tag;
            while (peek_token_is(Token::TOKEN_EQ))  // 下一个token 为'='
            {
                tag += m_curr.literal();
                next_token();  // class
                next_token();  // =
                tag += "=";
                tag += m_curr.literal();
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

//        while ()  // 获取field=value
//        {
//
//        }
    }
    else
    {
        std::ostringstream oss;
        oss << "Error : insert '"<< m_curr.literal() <<"' Syntax error";
        m_errors.push_back(oss.str());
    }
}