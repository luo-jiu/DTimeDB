#pragma once

#include "ast/node.h"

namespace dt::ast
{
    /**
     * 前缀表达式
     *
     * example:
     *      -a      负数
     */
    class Prefix : public Expression
    {
    public:
        Prefix(): Expression(NODE_PREFIX) {}
        ~Prefix() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["operator"] = m_operator;
            json["right"] = m_right->json();
            return json;
        }

    public:
        std::string m_operator;  // 运算符
        std::shared_ptr<Expression> m_right;  // 右表达式
    };
}
