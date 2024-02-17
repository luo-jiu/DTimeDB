#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;

/**
 * 将表达式转换成一条语句
 * @return
 */
std::shared_ptr<Statement> Parser::parse_statement()
{
    return parse_expression_statement();
}

std::shared_ptr<ExpressionStatement> Parser::parse_expression_statement()
{
    std::shared_ptr<ExpressionStatement> s(new ExpressionStatement());
    s->m_token = m_curr;
    s->m_expression = parse_expression(LOWEST);

    // 求值如果有错那么抛弃这个语句
    if (s->m_expression == nullptr)
    {
        return nullptr;
    }

    while (peek_token_is(Token::TOKEN_SEMICOLON))
    {
        next_token();
    }
    return s;
}