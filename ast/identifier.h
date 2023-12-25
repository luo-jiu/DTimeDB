#pragma once

#include <ast/node.h>

namespace dt::ast
{
    /**
     * 关键字节点
     */
    class Identifier : public Expression
    {
    public:
        Identifier(): Expression(NODE_IDENTIFIER) {}
        ~Identifier() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["value"] = m_value;
            return json;
        }

    public:
        string m_value;
    };
}
