#include <engine/tsm/tsm_ingredient.h>
using namespace dt::tsm;

bool DataBlock::write(
        high_resolution_clock::time_point timestamp,
        const string & value)
{
    if (timestamp.time_since_epoch() == high_resolution_clock::duration::zero())
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

string DataBlock::my_to_string(const string & value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}