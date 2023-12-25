#pragma once

#include <ast/node.h>
using namespace dt::ast;

namespace dt::ast
{
    /**
     * 浮点型节点
     */
    class Float : public Expression
    {
    public:
        Float(): Expression(NODE_FLOAT) {}
        ~Float() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["value"] = std::to_string(m_value);
            return json;
        }

    public:
        double m_value;
    };
}
