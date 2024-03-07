#include "data_point.h"
using namespace dt::tsm;

/**
 * 返回一个字段的值
 */
DataPoint::Value DataPoint::get_value(const std::string & field_name) const
{
    static Value empty;
    auto it = m_value_map.find(field_name);
    return it != m_value_map.end() ? it->second : empty;
}
