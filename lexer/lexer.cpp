#include "lexer/lexer.h"
using namespace dt::lexer;
using namespace dt::token;

using std::string;

Lexer::Lexer()
{
    m_ch = 0;
    m_pos = 0;
    m_next_pos = 0;
}

/**
 * 读取sql，给m_input 赋值，用于解析
 * @param file 方便写sql 测试，sql 文件路径
 */
Lexer::Lexer(const string & file)
{
    m_ch = 0;
    m_pos = 0;
    m_next_pos = 0;

    std::ifstream ifs(file);
    if (!ifs.good())  // 文件打开异常
    {
        std::cout << "Error: file not found - lexer/lexer.cpp" << std::endl;
        return;
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    m_input = oss.str();
}

/**
 * 对c 语言的支持
 */
Lexer::Lexer(const char * buf, int len)
{
    m_ch = 0;
    m_pos = 0;
    m_next_pos = 0;
    m_input.assign(buf, len);
}

/**
 * 拆分符号以及解析
 * 将其中的字符单位都解析成token 为单位
 * @return
 */
Token Lexer::next_token()
{
    read_char();
    skip_white_space();
    switch (m_ch)  // 根据不同字符进行不同的解析
    {
        case '+':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_PLUS, literal);
        }
        case '-':
        {
            if (peek_char() == '-')
            {
                string literal;
                literal += m_ch;
                read_char();
                literal += m_ch;
                return new_token(Token::TOKEN_DECREASE, literal);
            }
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_MINUS, literal);
        }
        case '*':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_ASTERISK, literal);
        }
        case '/':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_SLASH, literal);
        }
        case '%':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_MODULO, literal);
        }
        case '.':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_DOT, literal);
        }
        case ',':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_COMMA, literal);
        }
        case '<':
        {
            if (peek_char() == '=')  // <=
            {
                string literal;
                literal += m_ch;
                read_char();
                literal += m_ch;
                return new_token(Token::TOKEN_LE, literal);
            }
            else  // <
            {
                string literal;
                literal += m_ch;
                return new_token(Token::TOKEN_LT, literal);
            }
        }
        case '>':
        {
            if (peek_char() == '=')  // >=
            {
                string literal;
                literal += m_ch;
                read_char();
                literal += m_ch;
                return new_token(Token::TOKEN_GE, literal);
            }
            else  // >
            {
                string literal;
                literal += m_ch;
                return new_token(Token::TOKEN_GT, literal);
            }
        }
        case '=':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_EQ, literal);  // 是等于 不是赋值
        }
        case '(':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_LPAREN, literal);
        }
        case ')':
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_RPAREN, literal);
        }
        case '\'':  // 单引号
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_STRING, read_single_quote_string());
        }
        case '"':  // 双引号
        {
            string literal;
            literal += m_ch;
            return new_token(Token::TOKEN_STRING, read_double_quote_string());
        }
        case 0:  // EOF
        {
            return new_token(Token::TOKEN_EOF, "EOF");
        }
        default:  // 数字 或者 字面量
        {
            if (is_digit(m_ch))  // 数字
            {
                string integer = read_number();
                if (m_ch == '.' && is_digit(peek_char())) // 说明是小数,提取整数后,下一个符号是'.'且下一个字符是数字
                {
                    read_char();  // 跳过小数点
                    string fraction = read_number();  // 小数部分
                    unread_char();  // 调整指针
                    return new_token(Token::TOKEN_FLOAT, integer + "." + fraction);
                }
                unread_char();
                return new_token(Token::TOKEN_INTEGER, integer);
            }
            else if (is_letter(m_ch))  // 字面量
            {
                string literal = read_identifier();  // 读取标识符
                string temp = literal;

                // 转小写
                std::transform(literal.begin(), literal.end(), literal.begin(), [](unsigned char c)
                {
                    return std::tolower(c);
                });
                unread_char();

                Token::Type type = Token::look_up(literal);  // 通过标识符查找关键字
                if (type != Token::TOKEN_IDENTIFIER)  // 不是用户定义的
                {
                    return new_token(type, literal);
                }
                // 这里做了调整，抹去了变量，全部都转换成了字符串，原本应该是new_token(type, temp)
                return new_token(Token::TOKEN_STRING, temp);  // 用户定义的得原封不动
            }
            else  // 啥也不是就报错
            {
                string literal;
                literal += m_ch;
                return new_token(Token::TOKEN_ILLEGAL, literal);
            }
        }
    }
}

void Lexer::set_input(
        string & sql)
{
    m_input = sql;
}

/**
 * 跳过无用字符
 */
void Lexer::skip_white_space()
{
    while (m_ch == ' ' || m_ch == '\t' || m_ch == '\n' || m_ch == '\r')
    {
        read_char();  // 继续读取
    }
}

/**
 * 读取下一个字符
 */
void Lexer::read_char()
{
    if (m_next_pos >= m_input.length())
    {
        m_ch = 0;  // EOF
    }
    else
    {
        m_ch = m_input[m_next_pos];  // next char
    }
    m_pos = m_next_pos;
    ++m_next_pos;
}

/**
 * 获取下一个字符
 */
char Lexer::peek_char()
{
    if (m_next_pos >= m_input.length())
    {
        return 0;
    }
    else
    {
        return m_input[m_next_pos];  // next char
    }
}

/**
 * 读取数字后指针会有问题，需要调整指针
 */
void Lexer::unread_char()
{
    m_next_pos = m_pos;
    --m_pos;
}

Token Lexer::new_token(Token::Type type, const string & literal)
{
    Token token(type, literal);
    return token;
}

bool Lexer::is_digit(char ch)
{
    return (ch >= '0' && ch <= '9');
}

/**
 * 判断是不是有效标识符
 */
bool Lexer::is_letter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

/**
 * 获取数字
 * eg: ... m_where age < 30;
 * 获取30
 */
string Lexer::read_number()
{
    int pos = m_pos;
    while (is_digit(m_ch))
    {
        read_char();
    }
    return m_input.substr(pos, m_pos - pos);
}

/**
 * 获取标识符
 * eg: create ...
 * 获取create
 */
string Lexer::read_identifier()
{
    int pos = m_pos;
    while (is_letter(m_ch) || is_digit(m_ch))
    {
        read_char();
    }
    return m_input.substr(pos, m_pos - pos);
}

/**
 * 提取单引号中的字符串
 * eg: ... name = 'dt' ...
 * 获取 dt
 */
string Lexer::read_single_quote_string()
{
    string str;
    while (true)
    {
        read_char();
        if (m_ch == '\'' || m_ch == 0)
        {
            break;
        }
        else if (m_ch == '\\' && peek_char() == '\'')  // 转义字符 \'
        {
            read_char();
        }
        str += m_ch;
    }
    return str;
}

/**
 * 提取双引号中的字符串,与上同理
 */
string Lexer::read_double_quote_string()
{
    string str;
    while (true)
    {
        read_char();
        if (m_ch == '"' || m_ch == 0)
        {
            break;
        }
        else if (m_ch == '\\')
        {
            switch (peek_char())
            {
                case 'n':
                    read_char();
                    m_ch = '\n';
                    break;
                case 'r':
                    read_char();
                    m_ch = '\r';
                    break;
                case 't':
                    read_char();
                    m_ch = '\t';
                    break;
                case '"':
                    read_char();
                    m_ch = '"';
                    break;
                case '\\':
                    read_char();
                    m_ch = '\\';
                    break;
                default:
                    break;
            }
        }
        str += m_ch;
    }
    return str;
}
