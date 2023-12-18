#ifndef DTIMEDB_ERROR_H
#define DTIMEDB_ERROR_H

#include <object/object.h>

namespace dt::object
{
    class Error : public Object
    {
    public:
        Error() : Object(OBJECT_ERROR) {}
        Error(const string & message) : Object(OBJECT_ERROR), m_message(message) {}
        ~Error() {}

        virtual string str() const { return m_message; }

    public:
        string m_message;
    };
}

#endif //DTIMEDB_ERROR_H
