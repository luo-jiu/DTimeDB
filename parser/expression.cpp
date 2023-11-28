#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成树
 * @param precedence 传入优先级
 * @return
 */
std::shared_ptr<Expression> Parser::parse_expression(int precedence)
{
    auto prefix = m_prefix_parse_fns.find(m_curr.type());  // 前缀表达式
    if (prefix == m_prefix_parse_fns.end())  // 没找到
    {
        no_prefix_parse_fn_error(m_curr.type());
        return nullptr;
    }
    std::shared_ptr<Expression> e = (this->*(prefix->second))();  // 利用函数指针回调函数
    while (!peek_token_is(Token::TOKEN_SEMICOLON) && precedence < peek_token_precedence())  // 不是分号 && 优先级<下一个符号优先级
    {
        auto infix = m_infix_parse_fns.find(m_peek.type());  // 中缀表达式
        if (infix == m_infix_parse_fns.end())  // 没找到
        {
            return e;  // 说明是一个前缀表达式
        }
        next_token();
        e = (this->*(infix->second))(e);
    }
    return e;
}