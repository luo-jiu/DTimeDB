#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成ast/string_node 节点
 * @return ast/string_node
 */
std::shared_ptr<Expression> Parser::parse_string()
{
    std::shared_ptr<String> e(new String());
    e->m_token = m_curr;
    e->m_value = m_curr.literal();
    return e;
}

