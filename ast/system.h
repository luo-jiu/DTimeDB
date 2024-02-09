#ifndef DTIMEDB_SYSTEM_H
#define DTIMEDB_SYSTEM_H

#include <ast/node.h>

namespace dt::ast
{
    /**
     * 接收系统命令
     */
    class System : public Expression
    {
    public:
        System() : Expression(NODE_SYS) {}
        ~System() {}

        virtual Json json()
        {
            Json json;
            json["option"] = m_option;
            json["from"] = m_from;
            if (m_where != nullptr)
            {
                json["where"] = m_where->json();
            }
            else
            {
                json["where"] = "null";
            }
            return json;
        }

    public:
        string                                  m_option;
        string                                  m_from;
        std::shared_ptr<Expression>             m_where;
    };
}

#endif //DTIMEDB_SYSTEM_H
