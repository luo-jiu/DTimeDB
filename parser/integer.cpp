#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成ast 的integer 节点
 * @return ast/integer_node
 */
std::shared_ptr<Expression> Parser::parse_integer()
{
    std::shared_ptr<ast::Integer> e(new Integer());
    e->m_token = m_curr;
    e->m_value = std::atol(m_curr.literal().c_str());  // 将字符串转换为长整型数
    return e;
}