#include <object/object.h>
#include <object/integer.h>
#include <object/float.h>
#include <object/error.h>
#include <object/string.h>
#include <object/null.h>
#include <cstdarg>
using namespace dt::object;

std::map<Object::Type, string> Object::m_names = {
        {OBJECT_ERROR,       "error"},
        {OBJECT_STRING,      "string"},
        {OBJECT_INTEGER,     "integer"},
        {OBJECT_FLOAT,       "float"},
        {OBJECT_NULL,        "null"},
};

string Object::name() const
{
    auto it = m_names.find(m_type);
    if (it != m_names.end())
    {
        return it->second;
    }
    return "";
}

std::shared_ptr<Object> Object::new_error(const char * format, ...)
{
    char buf[1024] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(buf, sizeof(buf), format, arg_ptr);
    va_end(arg_ptr);

    std::shared_ptr<Error> obj = std::make_shared<Error>(buf);
    return obj;
}

std::shared_ptr<Object> Object::new_string(string & value)
{
    return std::make_shared<String>(value);
}

std::shared_ptr<Object> Object::new_integer(int64_t value)
{
    return std::make_shared<Integer>(value);
}

std::shared_ptr<Object> Object::new_float(float value)
{
    return std::make_shared<Float>(value);
}

std::shared_ptr<Object> Object::new_null()
{
    return std::make_shared<Null>();
}