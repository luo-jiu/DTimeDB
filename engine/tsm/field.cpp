#include <engine/tsm/field.h>
using namespace dt::tsm;

string Field::write(
        high_resolution_clock::time_point timestamp,
        string & data,
        string & db_name,
        string & tb_name)
{
    std::lock_guard<std::mutex> lock(m_sl_mutex);  // 直接上互斥锁
    m_sl.put(timestamp, data);

    if(m_sl.size() == 1)
    {
        m_sl_last_time = high_resolution_clock::now();
        std::cout << "初始化/重置m_sl_last_time, table: " << tb_name << std::endl;
        m_sl.notify(db_name, tb_name, true);
    }

    // 跳表是否需要刷新
    if (should_flush_data())
    {
        // 确保m_current_data 不为空
        if (!m_current_data)
        {
            m_current_data = std::make_shared<DataBlock>();
        }

        // 写入DataBlock
        int32_t _data_size = 0;
        if (m_type == DataBlock::Type::DATA_STRING)  // string
        {
            for (auto it = m_sl.begin(); it != m_sl.end(); ++it)  // 迭代器
            {
                if (it != m_sl.end())  // 校验
                {
                    auto key_value = *it;
                    m_current_data->write(key_value.first, key_value.second);
                    _data_size += (8 + key_value.second.length());
                }
            }
        }
        else
        {
            for (auto it = m_sl.begin(); it != m_sl.end(); ++it)  // 迭代器
            {
                if (it != m_sl.end())  // 校验
                {
                    auto key_value = *it;
                    m_current_data->write(key_value.first, key_value.second);
                }
            }
            _data_size += 12 * m_sl.size();
        }
        m_current_data->m_max_timestamp = m_sl.max_key();
        m_current_data->m_min_timestamp = m_sl.max_key();
        m_current_data->m_size = _data_size;  // 设置大小
        m_current_data->m_length = m_sl.size();  // 设置长度

        push_data_to_deque(m_current_data);  // 存放在队列中

        m_current_data = std::make_shared<DataBlock>();  // 重置m_current_data 以便接收新数据
        m_sl.cle();  // 清空跳表

        // 需要给write.h 中的m_fields_list中添加映射
        return m_field_name;
    }
    return "";
}

/**
 * 检查跳表是否需要刷新
 */
bool Field::should_flush_data()
{
    auto current_time = std::chrono::system_clock::now();
    return m_sl.size() >= 10 || (current_time - m_sl_last_time >= seconds(1) && !m_sl.empty());
}

/**
 * 放置数据
 */
void Field::push_data_to_deque(std::shared_ptr<DataBlock> data_block)
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    m_data_deque.push_back(data_block);
}

/**
 * 获取数据
 */
std::shared_ptr<DataBlock> Field::pop_data_from_deque()
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    if (!m_data_deque.empty())
    {
        std::shared_ptr<DataBlock> data_block = m_data_deque.front();
        m_data_deque.pop_front();
        return data_block;
    }
}

void Field::push_index_to_deque(std::shared_ptr<IndexEntry> & index_block)
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    m_index_deque.push_back(index_block);
}

std::shared_ptr<IndexEntry> Field::pop_index_from_deque()
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    if (!m_index_deque.empty())
    {
        std::shared_ptr<IndexEntry> index_block = m_index_deque.front();
        m_index_deque.pop_front();
        return index_block;
    }
}

SkipList<string> & Field::get_skip_list()
{
    return m_sl;
}

bool Field::get_data_status()
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    return m_data_deque.empty();
}

bool Field::get_index_status()
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    return m_index_deque.empty();
}

int Field::get_index_deque_size()
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    return m_index_deque.size();
}

