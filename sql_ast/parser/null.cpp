#include <parser/parser.h>
using namespace dt::parser;

/**
 * 生成ast/null_node 节点
 * @return ast/null_node
 */
std::shared_ptr<Expression> Parser::parse_null()
{
    std::shared_ptr<Null> e(new Null());
    e->m_token = m_curr;

    // null;  -->  null
    while (peek_token_is(Token::TOKEN_SEMICOLON))  // 如果是 ;
    {
        next_token();  // 跳过 ;
    }
    return e;
}