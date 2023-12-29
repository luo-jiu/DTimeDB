#include <parser/parser.h>
using namespace dt::parser;

std::shared_ptr<Expression> Parser::parse_create()
{
    std::shared_ptr<Create> e(new Create());
    next_token();
    if (m_curr.type() == Token::TOKEN_DATABASE)
    {
        // 由于create database xxx 没有其他的修饰，固然直接返回
        e->m_type = Create::CREATE_DATABASE;
        e->m_name = m_peek.literal();
        next_token();
        return e;
    }
    else if (m_curr.type() == Token::TOKEN_TABLE)
    {
        e->m_type = Create::CREATE_TABLE;
    }
    else
    {
        std::cerr << "Error : unknown token '" << m_curr.literal() << "'" << std::endl;
    }

    // 后面都是建表语句 create table xxx(field_1=varchar, field_2=float, field_3=integer)engine=clt;
    // 按道理TSM 是没有建表语句的,为了统一和CLT 引擎的切换，也做出调整
    // create table xxx(/* 注意没有任何字段 */)engine=tsm;
    e->m_name = m_peek.literal();
    next_token();
    next_token();

    if (m_curr.type() == Token::TOKEN_LPAREN)  // 如果是 '('
    {
        // 读取field 和类型
        while (m_curr.type() != Token::TOKEN_RPAREN || m_curr.type() == Token::TOKEN_EOF)  // 防止语法写错给代码崩了
        {
            next_token();
        }
        if (m_curr.type() == Token::TOKEN_EOF)
        {
            std::cerr << "Error : not fond ')';" << std::endl;
        }
    }

    // 出来就是 ')'
    next_token();  // skip ')'
    next_token();  // skip 'engine'
    next_token();  // skip '='
    if (m_curr.type() == Token::TOKEN_TSM || m_curr.type() == Token::TOKEN_CLT)
    {
        e->m_engine = m_curr.literal();
    }
    else
    {
        std::cerr << "Error : not fond engine;" << std::endl;
    }
    return e;
}