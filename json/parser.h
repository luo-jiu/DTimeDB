#pragma once

#include "json/json.h"

#include <fstream>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>

namespace dt::json
{
    class Parser
    {
    public:
        Parser() = default;
        ~Parser() = default;

        void load(const std::string & filename);
        void load(const char * buf, int len);
        Json parse();

    private:
        void skip_white_space();
        char get_next_token();

        Json parse_null();
        Json parse_bool();
        Json parse_number();
        std::string parse_string();
        Json parse_array();
        Json parse_object();

        bool in_range(int x, int lower, int upper)
        {
            return (x >= lower && x <= upper);
        }

    private:
        std::string m_str;
        size_t m_idx = 0;
    };
}

