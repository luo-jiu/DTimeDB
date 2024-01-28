#include <engine/tsm/controller.h>
using namespace dt::tsm;

/**
 * 初始化函数
 *
 * 主要目标是启动写入线程，监控线程
 */
void Controller::init()
{
    //  开启监控跳表线程
    m_monitor_thread = std::thread([this]{ this->monitoring_thread(); });
}

/**
 * 创建数据库
 *
 * 其实就是在真实的文件系统上创建了一个文件夹
 */
bool Controller::create_database(
        string & db_name)
{
    auto path = m_file.create_database(db_name);
    if (!path.empty())
    {
        return true;
    }
    return false;
}

/**
 * 当要操作一个表的时候才会调用
 */
void Controller::load_database(
        string & db_name)
{
    m_file.load_database(db_name);
}

void Controller::show_table(
        string & db_name)
{
    m_file.show_data(db_name);
}

/**
 * 创建表
 * @param tb_name
 * @return
 */
bool Controller::create_table(
        string & tb_name,
        string & db_name)
{
    // 先判断表是否已经存在
    if (m_file.exists_table(tb_name, db_name, false))
    {
        // 表已经存在
        std::cout << tb_name << " exists" << std::endl;
        return false;
    }

    // 创建表
    return m_file.create_table(tb_name, db_name, "tsm");
}

/**
 * 插入数据操作
 * @param file_path 文件路径
 * @return 操作是否成功
 */
bool Controller::insert(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & field_name,
        string & tb_name,
        string & db_name)
{
    // 从线程池分配任务
    m_producer_thread_pool.enqueue(&Controller::insert_thread, this, timestamp, value, type, field_name, tb_name, db_name);
    return true;
}

void Controller::insert_thread(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & field_name,
        string & tb_name,
        string & db_name)
{
    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
    std::cout << "tsm调用线程池处理插入任务..." << std::endl;

    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        if (!exists_table(db_name, tb_name))
        {
//            read_lock.unlock();
//            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            // 不存在需要初始化
            std::map<string, Table> _table;
            _table[tb_name] = Table{std::make_shared<Write>()};
            m_map[db_name] = Database{"", _table};

//            write_lock.unlock();
//            read_lock.lock();
        }
    }
    else
    {
        std::cout << "the '"<< tb_name << "' table does not exist.";
        return;
    }

    // 拿取出对应表的writer (这里一定是读取)
    auto & _writer = m_map[db_name].m_table_map[tb_name].m_writer;
    write_lock.unlock();
    // 类型转换
    DataBlock::Type _type;
    if (type == IEngine::Type::DATA_STRING)
    {
        _type = DataBlock::DATA_STRING;
    }
    else if (type == IEngine::Type::DATA_INTEGER)
    {
        _type = DataBlock::DATA_INTEGER;
    }
    else if (type == IEngine::Type::DATA_FLOAT)
    {
        _type = DataBlock::DATA_FLOAT;
    }
    else
    {
        std::cerr << "Error : unknown type " << type << std::endl;
        return;
    }
    _writer->write(timestamp, value, _type, field_name, db_name, tb_name, m_table_state, m_queue_state);
}

/**
 * 修改操作
 */
bool Controller::update(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & table_name)
{

}

bool Controller::get_next_data(string & data)
{

}

void Controller::begin_indexed_scan(
        const high_resolution_clock::time_point & timestamp,
        string & data)
{

}

bool Controller::get_range_datas(
        const high_resolution_clock::time_point & start,
        const high_resolution_clock::time_point & end,
        std::vector<string> & datas)
{

}

bool Controller::get_range_datas(
        std::vector<string> tags,
        std::vector<string> datas)
{

}

/**
 * 监控线程需要执行到函数
 *
 * 用于不断去检查跳表中是否有残余数据还未刷写
 */
void Controller::monitoring_thread()
{
    while(m_running.load())
    {
        m_table_state.iterate_map();
        m_queue_state.iterate_map();
        // 等待一段时间再次检查
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

/**
 * 结束监控线程
 */
void Controller::stop_monitoring_thread()
{
    m_running.store(false);
}

/**
 * 保证调用这个方法的方法已经有读写锁了
 */
bool Controller::exists_table(
        string & db_name,
        string & tb_name)
{
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            return true;
        }
    }
    return false;
}

/**
 * 残余数据刷盘
 */
bool Controller::is_ready_disk_write(
        const string & db_name,
        const string & tb_name,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            auto _writer = tb_it->second.m_writer;
            if (_writer)
            {
                std::cout << "---进入判断是否需要刷盘逻辑...\n";
                // 拿取到时间戳
                auto tp_it = tb_it->second.m_writer->get_field_time_point(field_name);
                auto current_time = system_clock::now();
                if (current_time - tp_it >= seconds(5))
                {
                    std::cout << "---满足刷盘\n";

                    // 刷跳表(传入空数据激活即可)
                    // 获取对应字段类型
                    auto _fd = field_name;
                    auto _tb = tb_name;
                    auto _db = db_name;
                    m_producer_thread_pool.enqueue(&Controller::insert_thread, this, system_clock::now(), "", Type::DATA_STRING, _fd, _tb, _db);

                    return true;  // 这里还有判断空没有写
                }
                else
                {
                    std::cout << "---本轮不满足刷盘\n";
                }
            }
        }
    }
    return false;
}

/**
 * 开启刷盘线程
 */
void Controller::disk_write_thread(
        const string & db_name,
        const string & tb_name,
        const string & field_name)
{
    std::cout << "================disk_write_thread==============" << std::endl;
}