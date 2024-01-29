#pragma once

#include <ast/node.h>

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
            for (auto & cla : fields)
            {
//                clause.append(cla->json());
                clause.append(cla);
            }
            json["fields"] = clause;
            json["from"] = m_from->json();
            json["where"] = m_where->json();
            return json;
        }

    public:
        std::list<string>                       fields;
        std::shared_ptr<Expression>             m_from;
        std::shared_ptr<Expression>             m_where;
    };
}