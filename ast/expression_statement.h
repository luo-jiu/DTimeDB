#pragma once

#include <ast/node.h>

namespace dt::ast
{
    /**
     * 表达式语句
     * 将一个表达式包装成一个语句
     */
    class ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement(): Statement(NODE_EXPRESSION_STATEMENT) {}
        ~ExpressionStatement() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["expression"] = m_expression->json();
            return json;
        }

    public:
        std::shared_ptr<Expression> m_expression;
    };
}