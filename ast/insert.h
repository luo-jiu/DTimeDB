#ifndef DTIMEDB_INSERT_H
#define DTIMEDB_INSERT_H

#include <ast/node.h>

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
            json["from"] = m_from->json();

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

            return json;
        }

    public:
        std::shared_ptr<Expression>         m_from;
        std::list<string>                   m_tags;       // 标签集合
        std::list<string>                   m_fields;     // 字段集合
        std::list<string>                   m_values;     // 值集合
        string                              m_timestamp;  // timestamp
    };
}

#endif //DTIMEDB_INSERT_H
