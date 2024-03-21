#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::ast;
using namespace dt::token;

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
        return nullptr;
    }

    // 后面都是建表语句 create table xxx(field_1 varchar, field_2 float, field_3 integer)engine=clt;
    // 按道理TSM 是没有建表语句的,为了统一和CLT 引擎的切换，也做出调整
    // create table xxx(/* 注意没有任何字段 */)engine=tsm;
    e->m_name = m_peek.literal();
    next_token();
    next_token();

    if (m_curr.type() == Token::TOKEN_LPAREN)  // 如果是 '('
    {
        // 读取field 和类型
        next_token();  // skip '('
        while (m_curr.type() != Token::TOKEN_RPAREN && m_curr.type() != Token::TOKEN_EOF)  // 防止语法写错给代码崩了
        {
            std::pair<std::string, std::string> field;
            field.first = m_curr.literal();
            next_token();
            field.second = m_curr.literal();
            next_token();
            e->m_fields.push_back(field);

            if (curr_token_is(Token::TOKEN_COMMA))  // 如果是 ‘,’
            {
                next_token();
            }
        }
        if (m_peek.type() == Token::TOKEN_RPAREN) next_token();
        if (m_curr.type() == Token::TOKEN_EOF)
        {
            std::cerr << "Error: n`ot fond ')';" << std::endl;
            while (m_curr.type() != Token::TOKEN_EOF) next_token();  // 清空语句
            return nullptr;
        }
    }
    else
    {
        // 语法不对,没有(),先报错,再结束掉剩下的语句
        std::cout << "Error: you have a syntax error in \"create table " << e->m_name << "'()'\";" << std::endl;
        while (m_curr.type() != Token::TOKEN_EOF) next_token();  // 清空语句
        return nullptr;
    }

    // 出来就是 ')'
    if (!peek_token_is(Token::TOKEN_ENGINE))
    {
        e->m_engine = "tsm";  // 默认引擎
        return e;
    }
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
        return nullptr;
    }
    return e;
}