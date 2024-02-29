#pragma once

#include "ast/node.h"

namespace dt::ast
{
    class Select : public Expression
    {
    public:
        Select(): Expression(NODE_SELECT) {}
        ~Select() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();

            Json clause;
            for (auto & cla : m_fields)
            {
//                clause.append(cla->json());
                clause.append(cla);
            }
            json["engine"] = m_engine;
            json["fields"] = clause;
            json["from"] = m_from->json();
            json["where"] = m_where->json();
            return json;
        }

    public:
        std::string                                             m_curr_db_name;  // 是否提前指定数据库
        std::string                                             m_engine;
        std::list<std::string>                                  m_fields;
        std::vector<std::pair<std::string, std::string>>        m_reduced_fields;  // 聚合函数以及要被聚合的字段
        std::shared_ptr<Expression>                             m_from;
        std::shared_ptr<Expression>                             m_where;
    };
}