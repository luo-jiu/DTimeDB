#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;

/**
 * 生成ast 的float 节点
 * @return ast/float_node
 */
std::shared_ptr<Expression> Parser::parse_float()
{
    std::shared_ptr<Float> e(new Float());  // 先创建float 的节点
    e->m_token = m_curr;
    e->m_value = std::atof(m_curr.literal().c_str());  // 将字符串转换为浮点数
    return e;
}