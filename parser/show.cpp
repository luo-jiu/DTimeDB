#include <parser/parser.h>
using namespace dt::parser;

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
    }
    next_token();
    return e;
}