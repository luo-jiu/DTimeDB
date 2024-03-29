#include "engine/tsm/field.h"
using namespace dt::tsm;
using namespace dt::impl;
using namespace std::chrono;
using std::string;

void Field::write(
        string & shard_id,
        high_resolution_clock::time_point timestamp,
        string & data,
        string & db_name,
        string & tb_name,
        const std::string & series_key_and_field_name)
{
    m_sl_mutex.lock();  // 先锁整个m_map[读锁]

    string combination_key = shard_id;
    if (!series_key_and_field_name.empty())
    {
        // 先简单拼一起，也能保证唯一性
        combination_key = shard_id + series_key_and_field_name;
        std::cout << "combination_key: " << combination_key << "\n";
    }
    // 检查分片是否存在
    auto shard_it = m_shard_skip_map.find(combination_key);
    if (shard_it == m_shard_skip_map.end())
    {
        // 分片不存在, 创建分片
        std::cout << "跳表该[分片]对应的[series key + field_name]不存在，创建分片\n";
        m_shard_skip_map[combination_key] = std::make_unique<SkipList<std::string>>();
    }
    // 释放全局锁,细化锁到对应分片的锁
    m_sl_mutex.unlock();
    std::unique_lock<std::shared_mutex> shard_sl_lock(m_shard_skip_locks[combination_key]);

    auto& sl = m_shard_skip_map[combination_key];
    if (!sl) return;

    /**
     * 重置数据模块
     */
    if (sl->m_need_reset.load())
    {
        sl->m_sl_last_time = high_resolution_clock::now();
        std::cout << "初始化/重置m_sl_last_time, table: " << tb_name << ", shard id:" << shard_id << ", field:" << m_field_name << std::endl;
        m_sl.notify(db_name, tb_name, {shard_id, series_key_and_field_name}, m_field_name, true, false);  // 注册事件
        std::cout << "注册事件, table: " << tb_name << ", field:" << m_field_name << std::endl;
    }

    /**
     * 跳表组织数据模块
     */
    if (!data.empty())
    {
        sl->put(timestamp, data);

        // 不需要重置数据
        sl->m_need_reset.store(false);
    }

    if (should_flush_data(sl->m_sl_last_time, sl->size()))
    {
        // 确保m_current_data 不为空
        if (!sl->m_current_data)
        {
            sl->m_current_data = std::make_shared<DataBlock>();
        }

        // 写入DataBlock
        int32_t data_size = 0;
        if (m_type == DataBlock::Type::DATA_STRING)  // string
        {
            for (auto it = sl->begin(); it != sl->end(); ++it)  // 迭代器
            {
                if (it != sl->end())  // 校验
                {
                    auto key_value = *it;
                    std::cout << "数据写入块" << key_value.second << "" << std::endl;
                    sl->m_current_data->write(key_value.first, key_value.second);
                    data_size += (8 + key_value.second.length());
                }
            }
        }
        else
        {
            for (auto it = sl->begin(); it != sl->end(); ++it)
            {
                if (it != sl->end())
                {
                    auto key_value = *it;
                    sl->m_current_data->write(key_value.first, key_value.second);
                }
            }
            data_size += 12 * sl->size();
        }
        // 设置参数
        sl->m_current_data->m_max_timestamp = sl->max_key();
        sl->m_current_data->m_min_timestamp = sl->min_key();
        sl->m_current_data->m_field_name = m_field_name;
        sl->m_current_data->m_size = data_size;
        sl->m_current_data->m_num = sl->size();
        sl->m_current_data->m_shard_id = shard_id;
        sl->m_current_data->series_key_and_field_name = series_key_and_field_name;

        // 存放在队列中
        push_data_to_deque(sl->m_current_data);

        // 注册事件，data block queue有元素
        notify(db_name, tb_name, {shard_id, ""}, m_field_name, true, false);

        // 重置m_current_data 以便接收新数据
        sl->m_current_data = std::make_shared<DataBlock>();
        // 清空跳表
        sl->cle();
        // 刷了块取消监控事件
        m_sl.notify(db_name, tb_name, {shard_id, series_key_and_field_name}, m_field_name, false, false);
        std::cout << "数据已入队列...\n";
        // 使下一次写入数据时初始化时间戳
        sl->m_need_reset.store(true);
    }
}

/**
 * 检查跳表是否需要刷新
 */
bool Field::should_flush_data(high_resolution_clock::time_point last_time, size_t sl_size)
{
    std::shared_lock<std::shared_mutex> read_lock(m_time_mutex);
    auto current_time = system_clock::now();
    return sl_size >= 100 || (current_time - last_time >= seconds(3) && sl_size > 0);
}

/**
 * 放置数据
 */
void Field::push_data_to_deque(std::shared_ptr<DataBlock> data_block)
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    // 回调将data block 写入到对应的writer
    m_data_block_callback(data_block);
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
bool Field::skip_need_flush_data_block(const string & shard_id)
{
    std::shared_lock<std::shared_mutex> read_lock(m_time_mutex);
    auto& sl_it = m_shard_skip_map[shard_id];
    if (sl_it)
    {
        return should_flush_data(sl_it->m_sl_last_time, sl_it->size());
    }
    return false;
}

bool Field::index_need_flush_disk()
{
    std::lock_guard<std::mutex> lock(m_index_lock);
//    if (m_index_deque.size() >=10 || );
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

bool Field::get_mate_status()
{
    return m_create_meta.load();
}

void Field::set_mate_status(bool state)
{
    m_create_meta.store(state);
}

/**
 * 添加观察者
 */
void Field::attach(ITableStateObserver * observer)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_observers.push_back(observer);
}

/**
 * 移除观察者
 */
void Field::detach(ITableStateObserver * observer)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    m_observers.remove(observer);
}

/**
 * 注册时间
 */
void Field::notify(
        const string & db_name,
        const string & tb_name,
        const std::pair<std::string, std::string> & shard_id_and_series_info,
        const string & field_name,
        bool is_registered,
        bool use_index_entry_map)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    // 通知所有观察者发生变化
    for (auto observer : m_observers)
    {
        observer->update(db_name, tb_name, shard_id_and_series_info, field_name, is_registered, use_index_entry_map);
    }
}