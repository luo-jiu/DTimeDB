#include <parser/parser.h>
using namespace dt::parser;

/**
 * 遇到'(' 才会调用该函数
 * @return
 */
std::shared_ptr<Expression> Parser::parse_group()
{
    next_token();  // 跳过'('
    auto e = parse_expression(LOWEST);
    if (!expect_peer_token(Token::TOKEN_RPAREN))  //  判断下一个符号是否为 ')'
    {
        return nullptr;
    }
    return e;
}