#include "token/token.h"
using namespace dt::token;
using std::string;

/**
 * 符号对应字面量表
 */
std::map<Token::Type, string> Token::m_names = {
        {TOKEN_ILLEGAL,         "illegal"},
        {TOKEN_EOF,             "eof"},
        {TOKEN_INTEGER,         "integer"},
        {TOKEN_FLOAT,           "float"},
        {TOKEN_STRING,          "string"},
        {TOKEN_PLUS,            "+"},
        {TOKEN_MINUS,           "-"},
        {TOKEN_ASTERISK,        "*"},
        {TOKEN_SLASH,           "/"},
        {TOKEN_MODULO,          "%"},
        {TOKEN_DECREASE,        "--"},
        {TOKEN_AND,             "AND"},
        {TOKEN_OR,              "OR"},
        {TOKEN_LT,              "<"},
        {TOKEN_GT,              ">"},
        {TOKEN_LE,              "<="},
        {TOKEN_GE,              ">="},
        {TOKEN_EQ,              "="},
        {TOKEN_NE,              "!="},
        {TOKEN_LPAREN,          "("},
        {TOKEN_RPAREN,          ")"},
        {TOKEN_DOT,             "."},
        {TOKEN_COMMA,           ","},
        {TOKEN_SEMICOLON,       ";"},
        {TOKEN_TIMESTAMP,       "timestamp"},

        {TOKEN_IDENTIFIER,      "identifier"},
        {TOKEN_NULL,            "null"},
        {TOKEN_DATABASE,        "database"},
        {TOKEN_DATABASES,       "databases"},
        {TOKEN_ENGINE,          "engine"},
        {TOKEN_TABLE,           "table"},
        {TOKEN_TABLES,          "tables"},
        {TOKEN_USE,             "use"},
        {TOKEN_SHOW,            "show"},
        {TOKEN_CREATE,          "create"},
        {TOKEN_INSERT,          "insert.cpp"},
        {TOKEN_INTO,            "into"},
        {TOKEN_UPDATE,          "update"},
        {TOKEN_SELECT,          "select"},

        {TOKEN_FROM,            "m_from"},
        {TOKEN_WHERE,           "m_where"},
        {TOKEN_TTL,             "ttl"},

        {TOKEN_TSM,             "tsm"},
        {TOKEN_CLT,             "clt"},

        {TOKEN_REDUCE,          "reduce"},
        {TOKEN_SYSTEM,          "system"},
};

/**
 * 关键字反查表
 */
std::map<string, Token::Type> Token::m_keywords = {
        {"null",            TOKEN_NULL},
        {"database",        TOKEN_DATABASE},
        {"databases",       TOKEN_DATABASES},
        {"table",           TOKEN_TABLE},
        {"tables",          TOKEN_TABLES},
        {"show",            TOKEN_SHOW},
        {"use",             TOKEN_USE},
        {"engine",          TOKEN_ENGINE},

        {"create",          TOKEN_CREATE},
        {"insert",          TOKEN_INSERT},
        {"into",            TOKEN_INTO},
        {"update",          TOKEN_UPDATE},
        {"select",          TOKEN_SELECT},

        {"from",            TOKEN_FROM},
        {"where",           TOKEN_WHERE},
        {"ttl",             TOKEN_TTL},

        {"and",             TOKEN_AND},
        {"or",              TOKEN_OR},

        {"tsm",             TOKEN_TSM},
        {"clt",             TOKEN_CLT},

        {"sum",             TOKEN_REDUCE},
        {"count",           TOKEN_REDUCE},
        {"mean",            TOKEN_REDUCE},
        {"median",          TOKEN_REDUCE},
        {"min",             TOKEN_REDUCE},
        {"max",             TOKEN_REDUCE},
        {"mode",            TOKEN_REDUCE},
        {"stddev",          TOKEN_REDUCE},
        {"percentile",      TOKEN_REDUCE},
        {"first",           TOKEN_REDUCE},
        {"last",            TOKEN_REDUCE},
        {"system",          TOKEN_SYSTEM},
};

Token::Token(): m_type(TOKEN_ILLEGAL)
{
}

Token::Token(Type type, const string & literal): m_type(type), m_literal(literal)
{
}

/**
 * 获取类型
 */
Token::Type Token::type() const
{
    return m_type;
}

/**
 * 获取当前符号字面量
 * 用于给通过类型赋予字面量初始值
 */
string Token::name() const
{
    auto it = m_names.find(m_type);
    if (it != m_names.end())
    {
        return it->second;
    }
    return "";
}

/**
 * 获取字面量
 */
string Token::literal() const
{
    return m_literal;
}

/**
 * Token赋值
 */
Token & Token::operator = (const Token & other)
{
    if (this == &other)
    {
        return *this;
    }
    m_type = other.m_type;
    m_literal = other.m_literal;
    return *this;
}

/**
 * 通过字面量 查询是否为关键字
 */
Token::Type Token::look_up(const string & literal)
{
    auto it = m_keywords.find(literal);
    if (it != m_keywords.end())
    {
        return it->second;
    }
    return TOKEN_IDENTIFIER;  // 用户定义的标识符/关键字
}

/**
 * 两个函数均为方便调试工具
 */
void Token::show() const
{
    printf("type=%d name=%s literal=%s\n", m_type, name().c_str(), literal().c_str());
}

Json Token::json() const
{
    Json json;
    json["type"] = name();
    json["name"] = m_literal;
    return json;
}