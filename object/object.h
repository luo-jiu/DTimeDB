#ifndef DTIMEDB_OBJECT_H
#define DTIMEDB_OBJECT_H

#include <memory>
#include <map>
#include <iostream>
using std::string;

namespace dt::object
{
    /**
     * 数据类型基类
     */
    class Object
    {
    public:
        enum Type
        {
            OBJECT_ERROR = 0,
            OBJECT_STRING,
            OBJECT_INTEGER,
            OBJECT_FLOAT,
            OBJECT_NULL,
        };

        Object() {}
        Object(Type type) : m_type(type) {}
        virtual ~Object() {}

        Type type() const { return m_type; }
        string name() const;
        virtual string str() const = 0;

        static std::shared_ptr<Object> new_error(const char * format, ...);
        static std::shared_ptr<Object> new_string(string & value);
        static std::shared_ptr<Object> new_integer(int64_t value);
        static std::shared_ptr<Object> new_float(float value);
        static std::shared_ptr<Object> new_null();
        static std::shared_ptr<Object> new_identifier();

    protected:
        Type                                m_type;
        static std::map<Type, string>       m_names;  // 类型 对应字符串名称
    };
}

#endif //DTIMEDB_OBJECT_H
