#include "parser/parser.h"
using namespace dt::parser;
using namespace dt::lexer;
using namespace dt::token;

/**
 * 运算符的优先级
 */
std::map<Token::Type, int> Parser::m_precedence = {
        {Token::TOKEN_PLUS,         SUM},
        {Token::TOKEN_MINUS,        SUM},  // '-'已经定义，负号不用再定义
        {Token::TOKEN_ASTERISK,     PRODUCT},
        {Token::TOKEN_SLASH,        PRODUCT},
        {Token::TOKEN_MODULO,       PRODUCT},
        {Token::TOKEN_AND,          LOGIC},
        {Token::TOKEN_OR,           LOGIC},
        {Token::TOKEN_LT,           COMPARE},
        {Token::TOKEN_GT,           COMPARE},
        {Token::TOKEN_LE,           COMPARE},
        {Token::TOKEN_GE,           COMPARE},
        {Token::TOKEN_EQ,           COMPARE},
        {Token::TOKEN_NE,           COMPARE},
};

/**
 * 前缀映射处理函数
 */
std::map<Token::Type, Parser::prefix_parse_fn> Parser::m_prefix_parse_fns = {
        {Token::TOKEN_INTEGER,    &Parser::parse_integer},
        {Token::TOKEN_FLOAT,      &Parser::parse_float},   // a.b
        {Token::TOKEN_LPAREN,     &Parser::parse_group},   // 左括号提升优先级
        {Token::TOKEN_MINUS,      &Parser::parse_prefix},  // -a
        {Token::TOKEN_STRING,     &Parser::parse_string},  // string
        {Token::TOKEN_NULL,       &Parser::parse_null},    // null
        {Token::TOKEN_IDENTIFIER, &Parser::parse_identifier},

        {Token::TOKEN_SHOW,       &Parser::parse_show},
        {Token::TOKEN_USE,        &Parser::parse_use},
        {Token::TOKEN_CREATE,     &Parser::parse_create},
        {Token::TOKEN_SELECT,     &Parser::parse_select},
        {Token::TOKEN_INSERT,     &Parser::parse_insert},
        {Token::TOKEN_SYSTEM,     &Parser::parse_system},
};

/**
 * 中缀映射处理函数
 */
std::map<Token::Type, Parser::infix_parse_fn> Parser::m_infix_parse_fns = {
        {Token::TOKEN_PLUS,          &Parser::parse_infix},
        {Token::TOKEN_MINUS,         &Parser::parse_infix},
        {Token::TOKEN_ASTERISK,      &Parser::parse_infix},
        {Token::TOKEN_SLASH,         &Parser::parse_infix},
        {Token::TOKEN_MODULO,        &Parser::parse_infix},
        {Token::TOKEN_LT,            &Parser::parse_infix},  // < > <= >= = !=
        {Token::TOKEN_GT,            &Parser::parse_infix},
        {Token::TOKEN_LE,            &Parser::parse_infix},
        {Token::TOKEN_GE,            &Parser::parse_infix},
        {Token::TOKEN_EQ,            &Parser::parse_infix},
        {Token::TOKEN_NE,            &Parser::parse_infix},
        {Token::TOKEN_AND,           &Parser::parse_infix},  // AND OR
        {Token::TOKEN_OR,            &Parser::parse_infix},
};

Parser::Parser()
{
}

Parser::Parser(const std::shared_ptr<Lexer> &lexer): m_lexer(lexer)
{
    // 两次调用next_token 初始化m_curr 和m_peek
    next_token();
    next_token();
}

Parser::~Parser()
{
}

void Parser::next_token()
{
    m_curr = m_peek;
    m_peek = m_lexer->next_token();  // 获取下一个token
}

/**
 * 比较当前的符号
 */
bool Parser::curr_token_is(Token::Type type)
{
    return m_curr.type() == type;
}

bool Parser::peek_token_is(Token::Type type)
{
    return m_peek.type() == type;
}

bool Parser::expect_peer_token(Token::Type type)
{
    if (peek_token_is(type))  // 判断下一个符号是否是我们期望的类型
    {
        next_token();
        return true;
    }
    else
    {
        peek_error(type);  // 记录错误
        return false;
    }
}

void Parser::peek_error(Token::Type type)
{
    std::ostringstream oss;
    oss << "Error: expected next token to be" << type << ", got " << m_peek.type() << " instead - parser/parser.cpp";
    m_errors.push_back(oss.str());
}

void Parser::no_prefix_parse_fn_error(Token::Type type)
{
    std::ostringstream oss;
    oss << "Error: no prefix parse function for - parser/parser.cpp" << type;
    m_errors.push_back(oss.str());
}

/**
 * 获取当前token 优先级
 */
int Parser::curr_token_precedence()
{
    auto it = m_precedence.find(m_curr.type());
    if (it != m_precedence.end())
    {
        return it->second;  // 返回优先级
    }
    return LOWEST;
}

int Parser::peek_token_precedence()
{
    auto it = m_precedence.find(m_peek.type());
    if (it != m_precedence.end())
    {
        return it->second;
    }
    return LOWEST;
}

std::list<std::string>& Parser::errors()
{
    return m_errors;
}