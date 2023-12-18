#ifndef DTIMEDB_INTEGRE_H
#define DTIMEDB_INTEGRE_H

#include <object/object.h>

namespace dt::object
{
    /**
     * 整形类型
     */
    class Integer : public Object
    {
    public:
        Integer() : Object(OBJECT_INTEGER) {}
        Integer(int64_t value) : Object(OBJECT_INTEGER), m_value(value) {}
        ~Integer() {}

        virtual string str() const { return std::to_string(m_value); }

    public:
        int64_t m_value;
    };
}

#endif //DTIMEDB_INTEGRE_H
