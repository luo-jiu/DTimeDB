#include <parser/parser.h>
using namespace dt::parser;

std::shared_ptr<Expression> Parser::parse_system()
{
    std::shared_ptr<System> s = std::make_shared<System>();
    s->m_token = m_curr;
    next_token();  // skip system
    s->m_option = m_curr.literal();
    next_token();  // skip option
    next_token();  // skip table
    s->m_from = m_curr.literal();
    if (!peek_token_is(Token::TOKEN_EOF))
    {
        next_token();
        next_token();
        s->m_where = parse_expression(LOWEST);
    }
    return s;
}