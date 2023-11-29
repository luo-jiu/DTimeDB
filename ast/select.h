#pragma once

#include <ast/node.h>

namespace dt
{
    namespace ast
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
                for (auto & cla : select_clause)
                {
                    clause.append(cla->json());
                }
                json["select_clause"] = clause;
                json["from"] = from->json();
                json["where"] = where->json();
                return json;
            }
        public:
            std::list<std::shared_ptr<Expression>>      select_clause;
            std::shared_ptr<Expression>                 from;
            std::shared_ptr<Expression>                 where;
        };
    }
}