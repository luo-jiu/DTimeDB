#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成ast/string_node 节点
 * @return ast/string_node
 */
std::shared_ptr<Expression> Parser::parse_select()
{
    std::shared_ptr<Select> e = std::make_shared<Select>();
    e->m_token = m_curr;
    next_token();  // 跳过select 关键字
    while (m_curr.type() != Token::TOKEN_FROM)  // 不是from
    {
        if (m_curr.type() == Token::TOKEN_ASTERISK) {  // 如果是*
//            std::shared_ptr<String> s(new String());
//            s->m_value = "*";
            e->fields.push_back("*");
            next_token();  // 跳过 '*'
            break;
        }
//        e->fields.push_back(parse_string());
        next_token();
        if(m_curr.type() == Token::TOKEN_COMMA){
            next_token();  // 跳过 ','
        }
    }
    next_token();
    e->m_from = parse_string();
    if (!peek_token_is(Token::TOKEN_EOF))  // 说明有where
    {
        next_token();
        next_token();
        e->m_where = parse_expression(LOWEST);
    }
    return e;
}

