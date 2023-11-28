#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成ast/string_node 节点
 * @return ast/string_node
 */
std::shared_ptr<Expression> Parser::parse_select()
{
    std::shared_ptr<Select> e(new Select());
    e->m_token = m_curr;
    next_token();  // 跳过select 关键字
    while (m_curr.type() != Token::TOKEN_FROM)  // 不是from
    {
        if (m_curr.type() == Token::TOKEN_ASTERISK) {  // 如果是*
            std::shared_ptr<String> s(new String());
            s->m_value = "*";
            e->select_clause.push_back(s);
            next_token();  // 跳过 '*'
            break;
        }
        e->select_clause.push_back(parse_string());
        next_token();
        if(m_curr.type() == Token::TOKEN_COMMA){
            next_token();  // 跳过 ','
        }
    }
    next_token();
    e->from = parse_string();
    next_token();
    next_token();
    e->where = parse_expression(LOWEST);
    return e;
}

