#ifndef DTIMEDB_NULL_H
#define DTIMEDB_NULL_H

#include <object/object.h>

namespace dt::object
{
    class Null : public Object
    {
    public:
        Null() : Object(OBJECT_NULL) {}
        ~Null() {}

        virtual string str() const { return "null"; }
    };
}

#endif //DTIMEDB_NULL_H
