#include <parser/parser.h>
using namespace dt::parser;

std::shared_ptr<Expression> Parser::parse_use()
{
    std::shared_ptr<Use> e = std::make_shared<Use>();
    next_token();  // skip use
    e->m_database_name = m_curr.literal();
    return e;
}