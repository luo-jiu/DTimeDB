#ifndef DTIMEDB_FLOAT_H
#define DTIMEDB_FLOAT_H

#include <object/object.h>

namespace dt::object
{
    class Float : public Object
    {
    public:
        Float() : Object(OBJECT_FLOAT) {}
        Float(float value) : Object(OBJECT_FLOAT), m_value(value) {}
        ~Float() {}

        virtual string str() const { return std::to_string(m_value); }

    public:
        float m_value;
    };
}

#endif //DTIMEDB_FLOAT_H
