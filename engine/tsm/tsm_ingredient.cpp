#include "engine/tsm/tsm_ingredient.h"
using namespace dt::tsm;

bool DataBlock::write(
        std::chrono::high_resolution_clock::time_point timestamp,
        const std::string & value)
{
    if (timestamp.time_since_epoch() == std::chrono::high_resolution_clock::duration::zero())
    {
        return false;
    }
    m_timestamps.push_back(timestamp);
    m_values.push_back(value);

    if(m_type == DATA_STRING)
    {
        m_size += (8 + value.length());  // 记录块大小
    }
    else
    {
        m_size += 12;
    }

    ++m_num;  // 记录数据数量
    return true;
}

std::string DataBlock::my_to_string(const std::string & value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}