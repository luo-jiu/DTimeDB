#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;

/**
 * 生成ast 的前缀表达式prefix 节点
 * @return ast/prefix_node
 */
std::shared_ptr<Expression> Parser::parse_prefix()
{
    std::shared_ptr<Prefix> e(new Prefix());
    e->m_token = m_curr;
    e->m_operator = m_curr.literal();
    next_token();
    e->m_right = parse_expression(PREFIX);
    return e;
}