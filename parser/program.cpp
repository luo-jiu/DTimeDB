#include <parser/parser.h>
using namespace dt::parser;

std::shared_ptr<Program> Parser::parse_program()
{
    std::shared_ptr<Program> s(new Program());
    while (m_curr.type() != Token::TOKEN_EOF)  // 不是结束符
    {
        auto stmt = parse_statement();
        if (stmt)
        {
            s->m_statements.push_back(stmt);
        }
        next_token();  // 后移知道遇见结束符
    }
    return s;
}
