#include <engine/tsm/field.h>
using namespace dt::tsm;

string Field::write(
        high_resolution_clock::time_point timestamp,
        string & data,
        string & db_name,
        string & tb_name)
{

    std::lock_guard<std::mutex> lock(m_sl_mutex);  // 直接上互斥锁
    /**
     * 重置数据模块
     */
    if(m_need_reset.load())
    {
        std::unique_lock<std::shared_mutex> write_lock(m_time_mutex);  // 采用写锁修改时间
        m_sl_last_time = high_resolution_clock::now();
        write_lock.unlock();

        std::cout << "初始化/重置m_sl_last_time, table: " << tb_name << ", field:" << m_field_name << std::endl;
        m_sl.notify(db_name, tb_name, m_field_name, true);  // 注册事件
        std::cout << "注册事件, table: " << tb_name << ", field:" << m_field_name << std::endl;
    }

    /**
     * 跳表组织数据模块
     */
    if (!data.empty())
    {
        m_sl.put(timestamp, data);
        m_need_reset.store(false);  // 不需要重置数据
    }

    /**
     * 跳表是否需要刷写到队列
     */
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

        std::cout << "数据已入队列...\n";
        m_need_reset.store(true);
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
    std::shared_lock<std::shared_mutex> read_lock(m_time_mutex);
    auto current_time = std::chrono::system_clock::now();
    return m_sl.size() >= 10 || (current_time - m_sl_last_time >= seconds(5) && !m_sl.empty());
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

/**
 * 获取跳表的时间戳
 * 采取读锁
 */
high_resolution_clock::time_point Field::get_skip_list_time_point()
{
    std::shared_lock<std::shared_mutex> read_lock(m_time_mutex);
    return m_sl_last_time;
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

