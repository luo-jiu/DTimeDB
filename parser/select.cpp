#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;

/**
 * 生成ast/string_node 节点
 * @return ast/string_node
 */
std::shared_ptr<Expression> Parser::parse_select()
{
    // select mean('温度') from school where local = '学校'
    std::shared_ptr<Select> e = std::make_shared<Select>();
    e->m_token = m_curr;
    next_token();  // 跳过select 关键字
    while (m_curr.type() != Token::TOKEN_FROM)  // 不是from
    {
        if (m_curr.type() == Token::TOKEN_ASTERISK) // 如果是*
        {
//            std::shared_ptr<String> s(new String());
//            s->m_value = "*";
            std::pair<std::string, std::string> field;
            field.first = "*";
//            e->m_fields.emplace_back("*");
            e->m_reduced_fields.push_back(field);
            next_token();  // 跳过 '*'
            break;
        }
//        e->fields.push_back(parse_string());
        // 判断是不是聚合函数
        if (curr_token_is(Token::TOKEN_REDUCE))
        {
            std::pair<std::string, std::string> reduce;
            reduce.second = m_curr.literal();
            next_token();
            next_token();
            reduce.first = m_curr.literal();
            next_token();
            next_token();
            e->m_reduced_fields.push_back(reduce);
            if (curr_token_is(Token::TOKEN_COMMA))
            {
                next_token();
            }
            continue;
        }
        if (curr_token_is(Token::TOKEN_STRING))
        {
            std::pair<std::string, std::string> field;
            field.first = m_curr.literal();
            field.second = "";
            e->m_reduced_fields.push_back(field);
            next_token();
            if(m_curr.type() == Token::TOKEN_COMMA)
            {
                next_token();  // 跳过 ','
            }
        }
    }
    next_token();
    if (peek_token_is(Token::TOKEN_DOT))  // 下一个符号是'.'
    {
        e->m_curr_db_name = m_curr.literal();
        next_token();
        if (!peek_token_is(Token::TOKEN_STRING))
        {
            return nullptr;
        }
        next_token();
    }
    e->m_from = parse_string();
    if (!peek_token_is(Token::TOKEN_EOF))  // 说明有where
    {
        next_token();
        next_token();
        e->m_where = parse_expression(LOWEST);
    }
    return e;
}

