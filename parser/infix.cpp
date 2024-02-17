#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;

/**
 * 生成ast 的中缀表达式infix 节点; 也就是生成一颗子树
 * 传入左值，右值需要再走parse_expression() 因为是从左至右开始分析的
 * @return ast/infix_node
 */
std::shared_ptr<Expression> Parser::parse_infix(
        const std::shared_ptr<Expression> &left)
{
    std::shared_ptr<Infix> e(new Infix);
    e->m_token = m_curr;
    e->m_operator = m_curr.literal();
    e->m_left = left;
    int precedence = curr_token_precedence();
    next_token();
    e->m_right = parse_expression(precedence);
    return e;
}