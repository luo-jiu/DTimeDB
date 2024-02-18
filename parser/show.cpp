#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;

std::shared_ptr<Expression> Parser::parse_show()
{
    std::shared_ptr<Show> e = std::make_shared<Show>();
    if (peek_token_is(Token::TOKEN_DATABASES))
    {
        // show databases
        e->m_type = "database";
        next_token();
    }
    else if (peek_token_is(Token::TOKEN_TABLES))
    {
        // show tables
        e->m_type = "table";
        next_token();
    }
    else if (peek_token_is(Token::TOKEN_STRING))
    {
        next_token();
        // show [db_name].[table_name] [option]
        if (peek_token_is(Token::TOKEN_DOT))
        {
            e->m_curr_db_name = m_curr.literal();
            next_token();
            if (!peek_token_is(Token::TOKEN_STRING))
            {
                return nullptr;
            }
            next_token();
        }
        // show [table_name] [option]

        e->m_tb_name = m_curr.literal();

        // [option]
        if (!peek_token_is(Token::TOKEN_DECREASE))
        {
            return nullptr;
        }
        else
        {
            next_token();  // skip tb_name
            next_token();  // skip "--"
            e->m_show_option = m_curr.literal();
        }
    }
    else
    {
        std::cout << "There is a syntax error here in \"show '" << m_peek.literal() << "'" << std::endl;
        while (m_curr.type() != Token::TOKEN_EOF) next_token();
        return nullptr;
    }
    return e;
}