#pragma once

#include <ast/node.h>

namespace dt
{
    namespace ast
    {
        /**
         * 整形节点定义
         */
        class Integer : public Expression
        {
        public:
            Integer(): Expression(NODE_INTEGER) {}
            ~Integer() {}

            virtual Json json()
            {
                Json json;
                json["type"] = name();
                json["value"] = std::to_string(m_value);
                return json;
            }

        public:
            int64_t m_value;
        };
    }
}