#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <token/token.h>

namespace dt::lexer
{
    /**
     * 词法分析器
     */
    class Lexer
    {
    public:
        Lexer();
        Lexer(const std::string & file);
        Lexer(const char * buf, int len);
        ~Lexer() = default;

        dt::token::Token next_token();
        void set_input(std::string & sql);
    private:
        void skip_white_space();
        void read_char();
        char peek_char();
        void unread_char();

        bool is_digit(char ch);
        bool is_letter(char ch);

        std::string read_number();
        std::string read_identifier();
        std::string read_single_quote_string();
        std::string read_double_quote_string();

        dt::token::Token new_token(dt::token::Token::Type type, const std::string & literal);
    private:
        std::string     m_input;     // 完整语句
        char            m_ch;        // 当前符号
        int             m_pos;       // 当前符号下标
        int             m_next_pos;  // 下一个符号下标
    };
}
