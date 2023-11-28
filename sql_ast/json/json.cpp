#include <json/json.h>
#include <json/parser.h>
using namespace luo::json;

Json::Json() : m_type(JSON_NULL)
{
}

Json::Json(Type type) : m_type(type)
{
    switch (m_type)
    {
        case JSON_NULL:
            break;
        case JSON_BOOL:
            m_value.m_bool = false;
            break;
        case JSON_INT:
            m_value.m_int = 0;
            break;
        case JSON_DOUBLE:
            m_value.m_double = 0.0;
            break;
        case JSON_STRING:
            m_value.m_string = new string("");
            break;
        case JSON_ARRAY:
            m_value.m_array = new std::vector<Json>();
            break;
        case JSON_OBJECT:
            m_value.m_object = new std::map<string, Json>();
        default:
            break;
    }
}

Json::Json(bool value) : m_type(JSON_BOOL)
{
    m_value.m_bool = value;
}

Json::Json(int value) : m_type(JSON_INT)
{
    m_value.m_int = value;
}

Json::Json(double value) : m_type(JSON_DOUBLE)
{
    m_value.m_double = value;
}

Json::Json(const char * value) : m_type(JSON_STRING)
{
    m_value.m_string = new string(value);
}

Json::Json(const string & value) : m_type(JSON_STRING)
{
    m_value.m_string = new string(value);
}

Json::Json(const Json & other) : m_type(JSON_NULL)
{
    copy(other);
}

Json::Json(Json && other)
{
    swap(other);
}

Json::~Json()
{
    clear();
}

Json::Type Json::type() const
{
    return m_type;
}

bool Json::is_null() const
{
    return m_type == JSON_NULL;
}

bool Json::is_bool() const
{
    return m_type == JSON_BOOL;
}

bool Json::is_int() const
{
    return m_type == JSON_INT;
}

bool Json::is_double() const
{
    return m_type == JSON_DOUBLE;
}

bool Json::is_string() const
{
    return m_type == JSON_STRING;
}

bool Json::is_array() const
{
    return m_type == JSON_ARRAY;
}

bool Json::is_object() const
{
    return m_type == JSON_OBJECT;
}

bool Json::as_bool() const
{
    if (m_type == JSON_BOOL)
    {
        return m_value.m_bool;
    }
    throw std::logic_error("json type error: not bool type");
}

int Json::as_int() const
{
    if (m_type == JSON_INT)
    {
        return m_value.m_int;
    }
    throw std::logic_error("json type error: not int type");
}

double Json::as_double() const
{
    if (m_type == JSON_DOUBLE)
    {
        return m_value.m_double;
    }
    throw std::logic_error("json type error: not double type");
}

string Json::as_string() const
{
    if (m_type == JSON_STRING)
    {
        return *(m_value.m_string);
    }
    throw std::logic_error("json type error: not string type");
}

void Json::copy(const Json & other)
{
    clear();
    m_type = other.m_type;
    switch (m_type)
    {
        case JSON_NULL:
        case JSON_BOOL:
        case JSON_INT:
        case JSON_DOUBLE:
            m_value = other.m_value;
            break;
        case JSON_STRING:
            {
                if (other.m_value.m_string != nullptr)
                {
                    m_value.m_string = new string(*(other.m_value.m_string));
                }
            }
            break;
        case JSON_ARRAY:
            {
                if (other.m_value.m_array != nullptr)
                {
                    m_value.m_array = new std::vector<Json>();
                    for (auto it = (other.m_value.m_array)->begin(); it != (other.m_value.m_array)->end(); ++it)
                    {
                        m_value.m_array->push_back(*it);
                    }
                }
            }
            break;
        case JSON_OBJECT:
            {
                if (other.m_value.m_object != nullptr)
                {
                    m_value.m_object = new std::map<string, Json>();
                    for (auto it = (other.m_value.m_object)->begin(); it != (other.m_value.m_object)->end(); ++it)
                    {
                        (*(m_value.m_object))[it->first] = it->second;
                    }
                }
            }
            break;
        default:
            break;
    }
}

void Json::swap(Json & other)
{
    Type type = m_type;
    Value value = m_value;
    m_type = other.m_type;
    m_value = other.m_value;
    other.m_type = type;
    other.m_value = value;
}

int Json::size() const
{
    switch (m_type)
    {
        case JSON_ARRAY:
            return (int)(m_value.m_array)->size();
        case JSON_OBJECT:
            return (int)(m_value.m_object)->size();
        default:
            break;
    }
    return -1;
}

bool Json::empty() const
{
    switch (m_type)
    {
        case JSON_NULL:
            return true;
        case JSON_ARRAY:
            return (m_value.m_array)->empty();
        case JSON_OBJECT:
            return (m_value.m_object)->empty();
        default:
            break;
    }
    return false;
}

void Json::clear()
{
    switch (m_type)
    {
        case JSON_NULL:
        case JSON_BOOL:
        case JSON_INT:
        case JSON_DOUBLE:
            break;
        case JSON_STRING:
            {
                if (m_value.m_string != nullptr)
                {
                    delete m_value.m_string;
                    m_value.m_string = nullptr;
                }
            }
            break;
        case JSON_ARRAY:
            {
                if (m_value.m_array != nullptr)
                {
                    for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); ++it)
                    {
                        it->clear();
                    }
                    delete m_value.m_array;
                    m_value.m_array = nullptr;
                }
            }
            break;
        case JSON_OBJECT:
            {
                if (m_value.m_object != nullptr)
                {
                    for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); ++it)
                    {
                        it->second.clear();
                    }
                    delete m_value.m_object;
                    m_value.m_object = nullptr;
                }
            }
            break;
        default:
            break;
    }
    m_type = JSON_NULL;
}

bool Json::has(int index) const
{
    if (m_type != JSON_ARRAY)
    {
        return false;
    }
    int size = (int)(m_value.m_array)->size();
    return (index >= 0) && (index < size);
}

bool Json::has(const char * key) const
{
    if (m_type != JSON_OBJECT)
    {
        return false;
    }
    return (m_value.m_object)->find(key) != (m_value.m_object)->end();
}

bool Json::has(const string & key) const
{
    return has(key.c_str());
}

Json Json::get(int index) const
{
    if (!has(index))
    {
        return Json();
    }
    return (m_value.m_array)->at(index);
}

Json Json::get(const char * key) const
{
    if (!has(key))
    {
        return Json();
    }
    return (*(m_value.m_object))[key];
}

Json Json::get(const string & key) const
{
    return get(key.c_str());
}

void Json::remove(int index)
{
    if (!has(index))
    {
        return;
    }
    (m_value.m_array)->at(index).clear();
    (m_value.m_array)->erase((m_value.m_array)->begin() + index);
}

void Json::remove(const char * key)
{
    if (m_type != JSON_OBJECT)
    {
        return;
    }
    auto it = (m_value.m_object)->find(key);
    if (it == (m_value.m_object)->end())
    {
        return;
    }
    it->second.clear();
    (m_value.m_object)->erase(it);
}

void Json::remove(const string & key)
{
    return remove(key.c_str());
}

void Json::append(const Json & value)
{
    if (m_type != JSON_ARRAY)
    {
        clear();
        m_type = JSON_ARRAY;
        m_value.m_array = new std::vector<Json>();
    }
    (m_value.m_array)->push_back(value);
}

void Json::append(Json && value)
{
    if (m_type != JSON_ARRAY)
    {
        clear();
        m_type = JSON_ARRAY;
        m_value.m_array = new std::vector<Json>();
    }
    (m_value.m_array)->push_back(std::move(value));
}

Json & Json::operator = (bool value)
{
    clear();
    m_type = JSON_BOOL;
    m_value.m_bool = value;
    return *this;
}

Json & Json::operator = (int value)
{
    clear();
    m_type = JSON_INT;
    m_value.m_int = value;
    return *this;
}

Json & Json::operator = (double value)
{
    clear();
    m_type = JSON_DOUBLE;
    m_value.m_double = value;
    return *this;
}

Json & Json::operator = (const char * value)
{
    clear();
    m_type = JSON_STRING;
    m_value.m_string = new string(value);
    return *this;
}

Json & Json::operator = (const string & value)
{
    clear();
    m_type = JSON_STRING;
    m_value.m_string = new string(value);
    return *this;
}

Json & Json::operator = (const Json & other)
{
    if (this == &other)
    {
        return *this;
    }
    copy(other);
    return *this;
}

Json & Json::operator = (Json && other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    swap(other);
    return *this;
}

bool Json::operator == (const Json & other)
{
    if (m_type != other.type())
    {
        return false;
    }
    switch (m_type)
    {
        case JSON_NULL:
            return true;
        case JSON_BOOL:
            return (m_value.m_bool == other.m_value.m_bool);
        case JSON_INT:
            return (m_value.m_int == other.m_value.m_int);
        case JSON_DOUBLE:
            return (m_value.m_double == other.m_value.m_double);
        case JSON_STRING:
            return *(m_value.m_string) == *(other.m_value.m_string);
        case JSON_ARRAY:
            return m_value.m_array == other.m_value.m_array;
        case JSON_OBJECT:
            return m_value.m_object == other.m_value.m_object;
        default:
            break;
    }
    return false;
}

bool Json::operator != (const Json & other)
{
    return !(*this == other);
}

Json & Json::operator [] (int index)
{
    if (m_type != JSON_ARRAY)
    {
        throw std::logic_error("type error: not array");
    }
    if (!has(index))
    {
        throw std::logic_error("array out of range");
    }
    return (m_value.m_array)->at(index);
}

Json & Json::operator [] (const char * key)
{
    if (m_type != JSON_OBJECT)
    {
        clear();
        m_type = JSON_OBJECT;
        m_value.m_object = new std::map<string, Json>();
    }
    return (*(m_value.m_object))[key];
}

Json & Json::operator [] (const string & key)
{
    return (*this)[key.c_str()];
}

Json::operator bool()
{
    return as_bool();
}

Json::operator bool() const
{
    return as_bool();
}

Json::operator int()
{

    return as_int();
}

Json::operator int() const
{
    return as_int();
}

Json::operator double()
{
    return as_double();
}

Json::operator double() const
{
    return as_double();
}

Json::operator string()
{
    return as_string();
}

Json::operator string() const
{
    return as_string();
}

void Json::load(const string & filename)
{
    Parser parser;
    parser.load(filename);
    *this = parser.parse();
}

void Json::load(const char * buf, int len)
{
    Parser parser;
    parser.load(buf, len);
    *this = parser.parse();
}

string Json::str() const
{
    std::stringstream ss;
    switch (m_type)
    {
        case JSON_NULL:
            ss << "null";
            break;
        case JSON_BOOL:
            if (m_value.m_bool)
            {
                ss << "true";
            }
            else
            {
                ss << "false";
            }
            break;
        case JSON_INT:
            ss << m_value.m_int;
            break;
        case JSON_DOUBLE:
            ss << m_value.m_double;
            break;
        case JSON_STRING:
            ss << "\"" << *(m_value.m_string) << "\"";
            break;
        case JSON_ARRAY:
            {
                ss << "[";
                for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); ++it)
                {
                    if (it != (m_value.m_array)->begin())
                    {
                        ss << ",";
                    }
                    ss << (*it).str();
                }
                ss << "]";
            }
            break;
        case JSON_OBJECT:
            {
                ss << "{";
                for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); ++it)
                {
                    if (it != (m_value.m_object)->begin())
                    {
                        ss << ",";
                    }
                    ss << "\"" << it->first << "\":" << it->second.str();
                }
                ss << "}";
            }
            break;
        default:
            break;
    }
    return ss.str();
}