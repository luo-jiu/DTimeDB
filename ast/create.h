#ifndef DTIMEDB_CREATE_H
#define DTIMEDB_CREATE_H

#include "ast/node.h"

namespace dt::ast
{
    class Create : public Expression
    {
    public:
        Create() : Expression(NODE_CREATE) {}

        enum Type
        {
            CREATE_DATABASE = 0,
            CREATE_TABLE
        };

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["name"] = m_name;
            return json;
        }

    public:
        Type                                                 m_type;    // 创建类型
        std::string                                          m_name;    // 名称
        std::list<std::pair<std::string, std::string>>       m_fields;  // 字段名
        std::string                                          m_engine;  // 引擎
    };
}

#endif //DTIMEDB_CREATE_H
