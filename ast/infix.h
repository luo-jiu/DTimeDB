#pragma once

#include "ast/node.h"

namespace dt::ast
{
    /**
     * 中缀表达式
     */
    class Infix : public Expression
    {
    public:
        Infix(): Expression(NODE_INFIX) {}
        ~Infix() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["operator"] = m_operator;
            json["left"] = m_left->json();
            json["right"] = m_right->json();
            return json;
        }


    public:  // 这里不封装set get为的是操作方便
        // 中缀表达式组成: 运算符 左表达式 右表达式
        std::string                      m_operator;
        std::shared_ptr<Expression>      m_left;  // 智能指针
        std::shared_ptr<Expression>      m_right;
    };
}

