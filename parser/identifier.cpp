#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;

/**
 * 生成ast/identifier_node
 * @return ast/identifier_node
 */
std::shared_ptr<Expression> Parser::parse_identifier()
{
    std::shared_ptr<Identifier> e(new Identifier());
    e->m_token = m_curr;
    e->m_value = m_curr.literal();
    return e;
}