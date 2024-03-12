#include "engine/tsm/write.h"
using namespace dt::tsm;
using namespace dt::file;
using namespace std::chrono;
using std::string;

void Write::init()
{
    auto sys_info =  m_file_path->load_system_info(m_db_name, m_tb_name);
    m_head_offset = sys_info.m_head_offset;
    m_tail_offset = sys_info.m_tail_offset;
    m_margin = sys_info.m_margin;
    if (m_margin == 0)  // 没有容量把路径置位空自动生成新文件(其他数据都会刷新)
        m_curr_file_path = "";
    else
        m_curr_file_path = sys_info.m_curr_file_path;
}

void Write::set_file_path_manager(FilePathManager * file_path_manager)
{
    m_file_path = file_path_manager;
}

void Write::write(
        high_resolution_clock::time_point timestamp,
        string & series_key,
        string & data,
        DataBlock::Type type,
        string & field_name,
        string & db_name,
        string & tb_name,
        FieldState & tb_state,
        TableState & queue_state,
        bool is_monitor_thread)
{
    std::shared_ptr<Field> field(new Field());
    // 寻找或创建field
    switch(type)
    {
        case DataBlock::DATA_STRING:
        {
            field = get_field(field_name, series_key, "string", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_INTEGER:
        {
            field = get_field(field_name, series_key, "integer", tb_state, queue_state);
            break;
        }
        case DataBlock::DATA_FLOAT:
        {
            field = get_field(field_name, series_key, "float", tb_state, queue_state);
            break;
        }
        default:
        {
            std::cerr << "Error : Unknown type " << type << std::endl;
            return;  // 返回
        }
    }

    // 在这里需要判断写入那个shard(传入shard ID)
    std::string shard_id = Write::timestamp_to_shard_key(system_clock::to_time_t(timestamp));
    // 先判断shard 是否存在
    {
        std::unique_lock<std::shared_mutex> write_lock(m_mea_shard_mutex);
        auto shard_it = m_mea_shard_map.find(shard_id);
        if (shard_it == m_mea_shard_map.end())
        {
            // 不存在则创建
            std::cout << "shard_id:" << shard_id << "不存在，开始创建write-shard\n";
            auto shard = std::make_unique<Shard>();
            shard->set_database_name(m_db_name);
            shard->set_measurement(m_tb_name);
            shard->set_shard_id(shard_id);
            m_mea_shard_map[shard_id] = std::move(shard);
        }
    }

    // 以及把shard中的当前文件传入写入程序中
    string series_key_and_field_name;
    if (is_monitor_thread)
    {
        // 监控线程需要把data 中暂存的series_key_and_field_name 取出
        series_key_and_field_name = data;
        data = "";
    }
    else
    {
        series_key_and_field_name = series_key + field_name;
        std::cout << "series_key_and_field_name:" << series_key_and_field_name << "\n";
    }

    field->write(shard_id, timestamp, data, db_name, tb_name, series_key_and_field_name);
}

void create_tsm_file_init(const std::unique_ptr<Shard> & shard, const string & new_tsm_file_path)
{
    shard->set_curr_file_path(new_tsm_file_path);
    shard->set_curr_file_margin(DATA_BLOCK_MARGIN - 13);
    shard->set_curr_file_head_offset(5);
    shard->set_curr_file_tail_offset(DATA_BLOCK_MARGIN - 8);

    const std::string & file_path = new_tsm_file_path;
    size_t last_slash_pos = file_path.find_last_of('/');
    std::string file_name;
    if (last_slash_pos != std::string::npos)
    {
        file_name = file_path.substr(last_slash_pos + 1);
    }
    else
    {
        file_name = file_path;
    }
    shard->add_tsm_file(file_name);
}


/**
 * 该方法有两个线程会进入使用
 * [监控线程] 和 [正常刷块线程]
 *
 * 监控线程自己进来data block 是没有数据的,是针对series index block 刷盘
 * 而正常刷块的线程都有可能做
 * 所以直接让监控线程进来，有data block 就一起刷盘了，没有也无所谓
 * 如果正常的线程进来data block被刷走了, 没有了数据就退出来了也无所谓
 *
 * 所以保证这个函数为单线程的即可，直接采用了互斥锁
 */
void Write::queue_flush_disk()
{
    std::lock_guard<std::mutex> lock(m_flush_disk_mutex);
    while(true)
    {
        /**
         * [模块一]
         *
         * 最先处理任务队列里面需要写index entry 的块
         * 如果队列里面有任务[有任务的的前提条件是 (数量达标)|| (时间达到)]
         *
         * 只有[监控线程] 和 [模块二] 发现了满足条件会写入任务队列
         */
        std::cout << "有线程进入刷盘函数queue_flush_disk>>>>>>>>>>>>>>>>\n";
        if (task_queue_empty())
        {
            std::cout << "任务队列为空<<<<<<<<<<<<<<<<\n";
        }

        if (!task_queue_empty())
        {
            // 获取任务名
            auto field_name = pop_task();
            auto& index_info = m_index_map[field_name];
            auto& shard_id = index_info.m_shard_id;
            // 获取分片信息
            auto& curr_shard = get_shard(shard_id);
            if (curr_shard == nullptr)
            {
                std::cout << "分片信息为空\n";
                break;
            }
            auto tail_offset = curr_shard->curr_file_tail_offset();
            if (index_info.m_index_deque.empty())  // 队列为空
            {
                std::cout << "字段："<< field_name << "field_name为空, 跳过本次刷盘\n";
                continue;
            }
            std::cout << ">>>开始刷写index entry, 字段:" << field_name << "\n";
            // 获取series_key + field_key
            string meta_key = m_field_map[field_name]->m_index_block_meta_key;
            // 计算大小(index(28) * index_num + meta(12 + (series_key + field_key).size()))
            auto size = 28 * index_info.m_index_deque.size() + 8 + meta_key.size();
            std::cout << "meta_key大小: " << meta_key.size() << "\n";
            std::cout << "tail_offset:" << tail_offset << "\n";
            // 生成meta
            auto meta = m_tsm.create_index_meta(m_field_map[field_name]->m_type, meta_key);
            // 偏移尾指针进行刷盘
            tail_offset -= size;
            std::cout << "tail_offset:" << tail_offset << " ,size:" << size << std::endl;
            // 给series index block 刷盘
            // 同步footer
            m_tsm.write_series_index_block_to_file(index_info.m_index_deque, meta, curr_shard->curr_file_path(), tail_offset);
            m_index_map.erase(field_name);
            // 同步尾指针
            curr_shard->set_curr_file_tail_offset(tail_offset);

            shard_notify(true);

            auto field_it = m_field_map.find(field_name);
            if (field_it == m_field_map.end())  // 没有这个field,为空
            {
                std::cout << "not find field:'" << field_name << "'" << std::endl;
                break;
            }
            auto field = field_it->second;
            field->set_mate_status(false);  // 重置meta
        }

        /**
         * [模块二]
         *
         * 刷写data block, 判断队列中是否有data block
         * 生成对应的index entry,注册监控事件，判断是否需要刷盘
         */
        if (m_data_deque.empty())
        {
            // 队列为空结束刷盘
            std::cout << "结束刷盘\n";
            break;
        }
        else  // 说明队列有元素
        {
            auto data_block = pop_data_from_deque();
            auto field_name = data_block->m_field_name;
            auto shard_id = data_block->m_shard_id;
            // 获取shard
            auto& curr_shard = get_shard(shard_id);
            if (curr_shard == nullptr)
            {
                std::cout << "分片信息为空-\n";
                return;
            }
            auto margin = curr_shard->curr_file_margin();
            auto head_offset = curr_shard->curr_file_head_offset();
            auto tail_offset = curr_shard->curr_file_tail_offset();
            auto curr_file_path = curr_shard->curr_file_path();

            auto field_it = m_field_map.find(field_name);
            if (field_it == m_field_map.end())  // 没有这个field,为空
            {
                std::cout << "not find field:'" << field_name << "'" << std::endl;
                break;
            }
            auto field = field_it->second;

            std::cout << "开始刷写块,field:" << field_name << std::endl;

            // 计算 压缩后的大小
            auto compress_timestamp = m_tsm.calculate_timestamp_size(data_block->m_timestamps);
            auto compress_val = m_tsm.calculate_val_size(data_block->m_values);
            auto predict_size = 16 + compress_timestamp.size() + compress_val.size();

            // 获取待写入的文件以及偏移量,还有容量是否足够(放在了sys-tb_name.tsm里面)
            bool need_create_file = false;
            // 先判断有没有能刷写的文件
            if (!curr_file_path.empty())
            {
                // 文件大小不够[至少要预留series index block 和data block 大小的空间]
                if (margin < (predict_size + 40 + field->m_index_block_meta_key.size()))
                {
                    need_create_file = true;  // 需要创建新文件
                    // 开始刷写所有的series index block 和更新footer
                    // 遍历map刷取
                    for (auto& indexs : m_index_map)
                    {
                        if (indexs.second.m_index_deque.empty())
                        {
                            continue;
                        }
                        else
                        {
                            string meta_key = m_field_map[field_name]->m_index_block_meta_key;
                            auto size = 28 * indexs.second.m_index_deque.size() + 8 + meta_key.size();
                            auto meta = m_tsm.create_index_meta(m_field_map[field_name]->m_type, meta_key);
                            m_tsm.write_series_index_block_to_file(indexs.second.m_index_deque, meta, curr_file_path, tail_offset);
                            tail_offset -= size;
                        }
                    }
                    m_index_map.clear();  // 清空map
                }
            }

            // 需要创建文件
            if (need_create_file || curr_file_path.empty())
            {
                curr_file_path = m_file_path->create_file(m_tb_name, m_db_name, "tsm");
                create_tsm_file_init(curr_shard, curr_file_path);
                shard_notify(true);

                std::cout << "创建文件:m_curr_file_path:" << curr_file_path << "\n";

                // 第一次创建需要写入头部信息 和尾部信息
                Header header(1, 1);
                m_tsm.write_header_to_file(header, curr_file_path);
                Footer footer(0);
                m_tsm.write_footer_to_file(footer, curr_file_path, DATA_BLOCK_MARGIN);

                // 初始化各项信息
                margin = DATA_BLOCK_MARGIN - 13;
                head_offset = 5;  // 跳过header
                tail_offset = DATA_BLOCK_MARGIN - 8;  // 跳过footer
            }

            // 到这里 肯定是有文件写，且大小也足够用的时候
            // 刷盘
            auto use_size = m_tsm.write_data_to_file(data_block->m_type, data_block->m_num, compress_timestamp, compress_val, curr_file_path, head_offset);
            std::cout << "压缩前大小:" << data_block->m_size << std::endl;
            std::cout << "压缩后大小:" << use_size << std::endl;

            // 创建index entry [唯一生成]
            auto entry = m_tsm.create_index_entry(data_block->m_max_timestamp, data_block->m_min_timestamp, head_offset, use_size);

            margin -= use_size;  // 去除大小
            head_offset += use_size;  // 指针偏移

            push_index_to_deque(shard_id, field_name, entry);  // 存入队列
            std::cout << "生成index entry,为其注册监控事件\n";
            field->notify(m_db_name, m_tb_name, {shard_id, ""}, field_name, true, true);  // 注册监控事件监测index entry
            margin -= 28;  // 减去entry 大小

            // 判断是否计算过meta,生成过则跳过
            if (!field->get_mate_status())
            {
                // 没生成过 || 已经刷到磁盘了
                margin -= (8 + field->m_index_block_meta_key.size());  // 减去meta 大小(目的是预留写入位置)
                field->set_mate_status(true);
                m_index_map[field_name].m_last_time = high_resolution_clock::now();  // 重置计时器
            }

            // 判断是否满足刷盘条件
            if (should_flush_index(field_name))
            {
                // 给任务队列添加任务循环到 [模块1]进行series index block 刷盘处理
                push_task(field_name);
            }
            // 同步头指针 和 剩余大小(因为上面已经给series index block 大小计算在内,[模块一不再同步])
            curr_shard->set_curr_file_margin(margin);
            curr_shard->set_curr_file_head_offset(head_offset);
            shard_notify(true);
        }
    }
}

std::shared_ptr<Field> Write::get_field(
        string & field_name,
        string & series_key,
        const string & type,
        FieldState & tb_state,
        TableState & queue_state)
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    auto it = m_field_map.find(field_name);
    if (it != m_field_map.end())  // 找到field
    {
        if (!it->second->get_status())  // 判断状态
        {
            // 监控线程到此止步
            return it->second;
        }
    }

    std::shared_ptr<Field> field;
    // 没合适的field
    if (type == "string")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_STRING, field_name);
    }
    else if (type == "integer")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_INTEGER, field_name);
    }
    else if (type == "float")
    {
        field = std::make_shared<Field>(DataBlock::Type::DATA_FLOAT, field_name);
    }
    else
    {
        std::cerr << "Error : Unknown type " << type << std::endl;
        return nullptr;
    }
    // 设置key
    string meta_key = series_key + field_name;
    std::cout << "Meta key: " << meta_key << std::endl;
    std::cout << "Key size:" << meta_key.size() << std::endl;
    field->m_index_block_meta_key = meta_key;

    // 注册观察者
    field->attach(&queue_state);
    field->attach(&tb_state);
    field->get_skip_list().attach(&tb_state);

    // 设置回调函数
    field->set_condition_callback([this](std::shared_ptr<DataBlock> & data_block) {
        return push_data_to_deque(data_block);
    });

    m_field_map[field_name] = field;
    return field;
}

bool Write::fields_empty()
{
    std::lock_guard<std::mutex> lock(m_write_mutex);
    return m_field_map.empty();
}

/**
 * index entry 是否满足刷盘
 */
bool Write::should_flush_index(
        const string & field_name)
{
    auto index_it = m_index_map.find(field_name);
    if (index_it != m_index_map.end())
    {
        auto current_time = system_clock::now();
        return index_it->second.m_index_deque.size() >= 10 ||
            (current_time - index_it->second.m_last_time >= seconds(5) &&
                !index_it->second.m_index_deque.empty());
    }
    return false;
}

void Write::push_back_field_list(const string & field)
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    m_field_list.push_back(field);
}

string Write::pop_front_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    if (m_field_list.empty())
    {
        return {};
    }

    string field = m_field_list.front();
    m_field_list.pop_front();
    return field;
}

/**
 * 返回data block 队列的状态
 */
bool Write::get_data_status()
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    return m_data_deque.empty();
}

/**
 * 向data block队列中放置数据
 */
void Write::push_data_to_deque(std::shared_ptr<DataBlock> & data_block)
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    m_data_deque.push_back(data_block);
}

/**
 * 获取data block 队列中的数据
 */
std::shared_ptr<DataBlock> Write::pop_data_from_deque()
{
    std::lock_guard<std::mutex> lock(m_data_lock);
    if (!m_data_deque.empty())
    {
        std::shared_ptr<DataBlock> data_block = m_data_deque.front();
        m_data_deque.pop_front();
        return data_block;
    }
    return {};
}

void Write::push_index_to_deque(
        const string & shard_id,
        const string & field_name,
        const std::shared_ptr<IndexEntry> & index_block)
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    if (m_index_map[field_name].m_shard_id.empty())
    {
        m_index_map[field_name].m_shard_id = shard_id;
    }
    m_index_map[field_name].m_index_deque.push_back(index_block);
}

std::shared_ptr<IndexEntry> Write::pop_index_from_deque(
        const string & field_name)
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    auto index_it = m_index_map.find(field_name);
    if (index_it != m_index_map.end())
    {
        std::shared_ptr<IndexEntry> index_entry = index_it->second.m_index_deque.front();
        index_it->second.m_index_deque.pop_front();
        return index_entry;
    }
    return {};
}

size_t Write::get_index_deque_size(
        const string & field_name)
{
    std::lock_guard<std::mutex> lock(m_index_lock);
    return m_index_map[field_name].m_index_deque.size();
}

bool Write::task_queue_empty()
{
    std::lock_guard<std::mutex> lock(m_task_deque_mutex);
    return m_index_task_queue.empty();
}

/**
 * 向index entry 待刷写队列注册任务
 */
void Write::push_task(const string & field_name)
{
    std::lock_guard<std::mutex> lock(m_task_deque_mutex);
    // 如果没有重复的任务注册
    if (m_task_set.insert(field_name).second)
    {
        m_index_task_queue.push_back(field_name);
    }
}

/**
 * 获取index entry 任务队列消息
 */
string Write::pop_task()
{
    std::lock_guard<std::mutex> lock(m_task_deque_mutex);
    if (m_index_task_queue.empty())
    {
        return "";
    }
    auto file_name = m_index_task_queue.front();
    m_index_task_queue.pop_front();
    m_task_set.erase(file_name);
    return file_name;
}

bool Write::empty_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    return m_field_list.empty();
}

int Write::size_field_list()
{
    std::lock_guard<std::mutex> lock(m_field_list_mutex);
    return m_field_list.size();
}

/**
 * 获取对应跳表的时间戳
 */
bool Write::skip_need_flush_data_block(
        const std::string & shard_id,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> lock(m_filed_map_mutex);  // 读锁
    auto field_it = m_field_map.find(field_name);
    if (field_it != m_field_map.end())  // 拿到对应Field
    {
        return field_it->second->skip_need_flush_data_block(shard_id);
    }
}

bool Write::index_need_flush_data_block(
        const string & field_name)
{

}

void Write::shard_attach(dt::impl::IShardStateObserver * observer)
{
    std::unique_lock<std::shared_mutex> lock(m_observer_mutex);
    m_observers.push_back(observer);
}

void Write::shard_detach(dt::impl::IShardStateObserver * observer)
{
    std::shared_lock<std::shared_mutex> lock(m_observer_mutex);
    m_observers.remove(observer);
}

void Write::shard_notify(bool is_registered)
{
    std::shared_lock<std::shared_mutex> lock(m_observer_mutex);
    // 通知所有观察者发生变化
    for (auto observer : m_observers)
    {
        observer->shard_update(is_registered);
    }
}

/**
 * 通过时间戳计算shard key
 */
std::string Write::timestamp_to_shard_key(
        std::time_t timestamp)
{
    std::tm* tm = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::time_t Write::shard_key_to_timestamp(const std::string & shard_id)
{
    std::tm tm = {};
    std::istringstream ss(shard_id);

    // 使用std::get_time来从字符串解析时间，注意格式需要与timestamp_to_shard_key函数中的输出格式匹配
    ss >> std::get_time(&tm, "%Y-%m-%d");

    // 检查是否成功解析
    if(ss.fail())
    {
        throw std::runtime_error("Failed to parse the shard key");
    }

    // 将std::tm转换为std::time_t
    // 注意：这里假设tm表示的是本地时间
    std::time_t timestamp = std::mktime(&tm);

    // 检查转换结果
    if (timestamp == -1)
    {
        throw std::runtime_error("Conversion to time_t failed");
    }

    return timestamp;
}

std::list<std::string> Write::get_shard_in_meta()
{
    std::unique_lock<std::shared_mutex> write_lock(m_mea_shard_mutex);
    std::list<std::string> shard_serializes;
    for (auto& shard : m_mea_shard_map)
    {
        string val_serialize;
        std::cout << shard.second->DebugString() << "\n";
        shard.second->SerializeToString(&val_serialize);
        shard_serializes.push_back(val_serialize);
    }
    return shard_serializes;
}

std::vector<std::string> Write::get_shard_ids()
{
    std::unique_lock<std::shared_mutex> write_lock(m_mea_shard_mutex);
    std::vector<std::string> shard_ids;
    shard_ids.reserve(m_mea_shard_map.size());
    for (auto& shard : m_mea_shard_map)
    {
        shard_ids.push_back(shard.second->shard_id());
    }
    return shard_ids;
}

bool Write::add_shard(const std::string & shard_id, std::unique_ptr<Shard> & shard)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mea_shard_mutex);
    if (!shard_id.empty() && shard != nullptr)
    {
        m_mea_shard_map[shard_id] = std::move(shard);
        return true;
    }
    return false;
}

std::unique_ptr<Shard> & Write::get_shard(const std::string & shard_id)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mea_shard_mutex);

    auto shard = m_mea_shard_map.find(shard_id);
    if (shard != m_mea_shard_map.end())
    {
        std::cout << "get_shard:" << shard_id << ", shard_size:" << m_mea_shard_map.size() << ", curr_shard_id:" << shard->second->shard_id() << "\n";
        return shard->second;
    }
}