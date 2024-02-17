#pragma once

#include "ast/node.h"

namespace dt::ast
{
    /**
     * 空值定义
     * 空值没有成员变量
     */
    class Null : public Expression {
    public:
        Null() : Expression(NODE_NULL) {}
        ~Null() {}

        virtual Json json() {
            Json json;
            json["type"] = name();
            return json;
        }
    };
}
