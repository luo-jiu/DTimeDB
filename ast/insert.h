#ifndef DTIMEDB_INSERT_H
#define DTIMEDB_INSERT_H

#include "ast/node.h"

namespace dt::ast
{
    class Insert : public Expression
    {
    public:
        Insert() : Expression(NODE_INSERT) {}
        ~Insert() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["from"] = m_from;

            Json clause_tags;
            for (auto & cla : m_tags)
            {
                clause_tags.append(cla);
            }
            json["tags"] = clause_tags;

            Json clause_fields;
            for (auto & cla : m_fields)
            {
                clause_fields.append(cla);
            }
            json["fields"] = clause_fields;

            Json clause_values;
            for (auto & cla : m_values)
            {
                clause_values.append(cla);
            }
            json["values"] = clause_values;

            json["timestamp"] = m_timestamp;
            return json;
        }

    public:
        std::string                                      m_curr_db_name;  // 是否提前指定数据库
        std::string                                      m_from;
        std::list<std::string>                           m_tags;          // 标签集合
        std::list<std::string>                           m_fields;        // 字段集合
        std::list<std::string>                           m_values;        // 值集合
        std::string                                      m_timestamp;     // timestamp
    };
}

#endif //DTIMEDB_INSERT_H
