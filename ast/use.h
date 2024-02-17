#ifndef DTIMEDB_USE_H
#define DTIMEDB_USE_H

#include "ast/node.h"

namespace dt::ast
{
    class Use : public Expression
    {
    public:
        Use() : Expression(NODE_USE) {}
        Use(const std::string & database_name) : Expression(NODE_USE), m_database_name(database_name) {}
        ~Use() {}

        virtual Json json()
        {
            Json json;
            json["type"] = name();
            json["database"] = m_database_name;
            return json;
        }

    public:
        std::string m_database_name;
    };
}

#endif //DTIMEDB_USE_H
