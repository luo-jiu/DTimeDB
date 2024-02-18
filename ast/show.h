#ifndef DTIMEDB_SHOW_OPERATOR_H
#define DTIMEDB_SHOW_OPERATOR_H

#include "ast/node.h"

namespace dt::ast
{
    class Show : public Expression
    {
    public:
        Show(): Expression(NODE_SHOW) {}
        ~Show() {}

        virtual Json json()
        {
            Json json;
            json["type"] = m_type;
            return json;
        }

    public:
        std::string m_curr_db_name;  // 是否提前指定数据库
        std::string m_show_option;  // 显示选项
        std::string m_tb_name;
        std::string m_type;  // database or table
    };
}

#endif //DTIMEDB_SHOW_OPERATOR_H
