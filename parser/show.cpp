#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;

std::shared_ptr<Expression> Parser::parse_show()
{
    std::shared_ptr<Show> e = std::make_shared<Show>();
    if (peek_token_is(Token::TOKEN_DATABASES))
    {
        e->m_type = "database";
    }
    else if (peek_token_is(Token::TOKEN_TABLES))
    {
        e->m_type = "table";
    }
    else
    {
        std::cout << "There is a syntax error here in \"show '" << m_peek.literal() << "'" << std::endl;
        while (m_curr.type() != Token::TOKEN_EOF) next_token();
        return nullptr;
    }
    next_token();
    return e;
}