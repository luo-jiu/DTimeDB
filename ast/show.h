#ifndef DTIMEDB_SHOW_OPERATOR_H
#define DTIMEDB_SHOW_H

#include <ast/node.h>
using namespace dt::ast;

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
            json["type"] = name();
            json["type"] = m_type;
            return json;
        }

    public:
        string m_type;  // database or table
    };
}

#endif //DTIMEDB_SHOW_OPERATOR_H
