#ifndef DTIMEDB_STRING_H
#define DTIMEDB_STRING_H

#include <object/object.h>

namespace dt::object
{
    /**
     * 字符串类型
     */
    class String : public Object
    {
    public:
        String() : Object(OBJECT_STRING) {}
        String(string value) : Object(OBJECT_STRING), m_value(value) {}
        ~String() {}

        virtual string str() const { return m_value; }

    public:
        string m_value;
    };
}

#endif //DTIMEDB_STRING_H
