#pragma once

#include <ast/node.h>

namespace dt::ast
{
    /**
     * 字符串类定义
     */
    class String : public Expression
    {
    public:
        String(): Expression(NODE_STRING) {}
        ~String() {}

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
