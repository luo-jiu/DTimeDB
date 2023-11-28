#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <token/token.h>
using namespace dt::token;

namespace dt
{
    namespace lexer
    {
        /**
         * 词法分析器
         */
        class Lexer
        {
        public:
            Lexer();
            Lexer(const string & file);
            Lexer(const char * buf, int len);
            ~Lexer() = default;

            Token next_token();

        private:
            void skip_white_space();
            void read_char();
            char peek_char();
            void unread_char();

            bool is_digit(char ch);
            bool is_letter(char ch);

            string read_number();
            string read_identifier();
            string read_single_quote_string();
            string read_double_quote_string();

            Token new_token(Token::Type type, const string & literal);
        private:
            string      m_input;     // 完整语句
            char        m_ch;        // 当前符号
            int         m_pos;       // 当前符号下标
            int         m_next_pos;  // 下一个符号下标
        };
    }
}