#include <engine/tsm/controller.h>
using namespace dt::tsm;
using namespace dt::impl;
using namespace std::chrono;
using std::string;

Controller::Controller(): m_producer_thread_pool(8), m_consumer_thread_pool(6), m_running(true), m_file("tsm")
{
    init();
    // 设置回调函数
    m_field_state.set_skip_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name, const std::string & shard_id, const std::string & field_name) {
                // skip_list 数据监控 回调处理函数
                return is_ready_disk_write(db_name, tb_name, shard_id, field_name);
            });
    m_field_state.set_index_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name, const std::string & shard_id, const std::string & field_name) {
                // index queue监控 回调处理函数
                return is_ready_index_write(db_name, tb_name, shard_id, field_name);
            });
    m_table_state.set_condition_callback(
            [this](const std::string & db_name, const std::string & tb_name) {
                // data block queue监控 回调处理函数
                return disk_write(db_name, tb_name);
            });
}

Controller::~Controller()
{
    stop_monitoring_thread();
    if (m_monitor_thread.joinable())
    {
        m_monitor_thread.join();
    }
}

/**
 * 初始化函数
 *
 * 主要目标是启动写入线程，监控线程
 */
void Controller::init()
{
    //  开启监控线程
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
bool Controller::load_database(
        string & db_name,
        std::vector<std::string> & tables)
{
    return m_file.load_database(db_name, tables);
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
    if (m_file.exists_table(db_name, tb_name, false))
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
        string & tags_str,
        string value,
        Type type,
        string & field_name,
        string & tb_name,
        string & db_name)
{
    // 从线程池分配任务
    m_producer_thread_pool.enqueue(&Controller::insert_thread, this, timestamp, tags_str, value, type, field_name, tb_name, db_name);
    return true;
}

void Controller::insert_thread(
        high_resolution_clock::time_point timestamp,
        const string & tags_str,
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
            // 不存在需要初始化
            std::map<string, Table> table;
            auto write = std::make_shared<Write>(db_name, tb_name);
            write->set_file_path_manager(&m_file);  // 依赖注入
            write->init();  // 初始化
            table[tb_name] = Table{write};
            m_map[db_name] = Database{"", table};
        }
    }
    else
    {
        std::cout << "the '"<< tb_name << "' table does not exist.";
        return;
    }

    // 拿取出对应表的writer (这里一定是读取)
    auto & writer = m_map[db_name].m_table_map[tb_name].m_writer;
    write_lock.unlock();
    // 类型转换
    DataBlock::Type type_temp;
    if (type == IEngine::Type::DATA_STRING)
    {
        type_temp = DataBlock::DATA_STRING;
    }
    else if (type == IEngine::Type::DATA_INTEGER)
    {
        type_temp = DataBlock::DATA_INTEGER;
    }
    else if (type == IEngine::Type::DATA_FLOAT)
    {
        type_temp = DataBlock::DATA_FLOAT;
    }
    else
    {
        std::cerr << "Error : unknown type " << type << std::endl;
        return;
    }

    // 判断field 是否已经存在
    if (!mea_field_exist(db_name, tb_name, field_name))
    {
        // 不存在
        std::cout << "字段不存在，写入表元数据,field " << field_name << std::endl;
        m_file.insert_field_to_file(db_name, tb_name, field_name);
        add_field(db_name, tb_name, field_name);
    }

    // 拼接seriesKey
    string series_key = field_name + tags_str;
    writer->write(timestamp, series_key, value, type_temp, field_name, db_name, tb_name, m_field_state, m_table_state);
}

/**
 * 创建索引
 */
bool Controller::create_index(
        string & measurement,
        std::list<string> & tags)
{
    // 从线程池分配任务
    m_producer_thread_pool.enqueue(&Controller::create_index_thread, this, measurement, tags);
    return true;
}

void Controller::create_index_thread(
        string & measurement,
        std::list<string> & tags)
{
    m_index.create_index(measurement, tags);
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

bool Controller::get_range_data(
        const high_resolution_clock::time_point & start,
        const high_resolution_clock::time_point & end,
        std::vector<string> & data)
{

}

/**
 * 查询数据
 * @param measurement 测量值
 * @param field 字段集合
 * @param tags 纬度值
 * @param expr_node 表达式树
 */
bool Controller::get_range_data(
        const string & db_name,
        const string & measurement,
        std::vector<string> field,
        std::shared_ptr<ExprNode> expr_node)
{
    m_producer_thread_pool.enqueue(&Controller::analytic_expr_tree, this, db_name, measurement, field, expr_node);
    return true;
}

/**
 * 分离出tag 以及where
 */
void Controller::analytic_expr_tree(
        const std::string & db_name,
        const std::string & measurement,
        std::vector<std::string> field,
        const std::shared_ptr<impl::ExprNode> & expr_node)
{
    // 解析表达式树(摘除掉tag 的部分)
    std::vector<std::pair<std::string, std::string>> tags;
    auto new_expr_tree = rebuild_tree_without_tags(measurement, expr_node, tags);

    // 根据每一个字段分别构建 字段迭代器
    m_file.load_mea_fields(db_name, measurement);
}

/**
 * 重构表达式树
 * @param expr_node
 * @return
 */
std::shared_ptr<ExprNode> Controller::rebuild_tree_without_tags(
        const std::string & measurement,
        const std::shared_ptr<ExprNode> & expr_node,
        std::vector<std::pair<std::string, std::string>> & tags)
{
    if (!expr_node)
    {
        return nullptr;
    }
    // 如果当前比较属于纬度比较，返回空，移除该节点
    if (is_tag_comparison(measurement, expr_node))
    {
        tags.emplace_back(expr_node->m_left->m_val, expr_node->m_right->m_val);
        return nullptr;
    }
    auto left = rebuild_tree_without_tags(measurement, expr_node->m_left, tags);
    auto right = rebuild_tree_without_tags(measurement, expr_node->m_right, tags);

    if ((expr_node->m_val == "and" || expr_node->m_val == "or") && (!left || !right))
    {
        return left ? left : right;
    }
    auto new_node = std::make_shared<ExprNode>(*expr_node);
    new_node->m_left = left;
    new_node->m_right = right;
    return new_node;
}

bool Controller::is_tag_comparison(
        const std::string & measurement,
        const std::shared_ptr<ExprNode> & node)
{
    if (node && node->m_val == "=")
    {
        return m_index.whether_tag(measurement, node->m_left->m_val);
    }
    return false;
}

/**
 * 范围扫描拿出集合
 */
std::list<string> Controller::scan_full_table(
        const string & db_name,
        const string & tb_name)
{
    std::cout << "Scan full table, db_name:'" << db_name << "',tb_name:" << tb_name << std::endl;
    // 单独开启一个线程去执行
    std::list<string> result;
    return result;
}

bool Controller::sys_show_file(
        string & db_name,
        string & tb_name)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        return m_file.sys_show_file(db_name, tb_name);
    }
    return false;
}

bool Controller::sys_update_file(
        string & db_name,
        string & tb_name,
        const string & where)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        size_t pos = where.find('=');
        string key, value;
        if (pos != std::string::npos) {
            key = where.substr(0, pos);
            value = where.substr(pos + 1);
        }
        if (key == "head_offset")
        {
            return m_file.update_system_file_offset(db_name, tb_name, "head", std::stoi(value));
        }
        if (key == "tail_offset")
        {
            return m_file.update_system_file_offset(db_name, tb_name, "tail", std::stoi(value));
        }
        else if (key == "margin")
        {
            return m_file.update_system_file_margin(db_name, tb_name, std::stoi(value));
        }
    }
    return false;
}

bool Controller::sys_clear_file(
        string & db_name,
        string & tb_name)
{
    // 先看文件管理器里面有没有
    if (m_file.exists_table(db_name, tb_name, true))
    {
        return m_file.sys_clear_file(db_name, tb_name);
    }
    return false;
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
        m_field_state.iterate_map(false);  // 监控skip list
        m_field_state.iterate_map(true);   // 监控index entry
        m_table_state.iterate_map();       // 监控data deque
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
 * 从系统文件中加载数据库字段
 */
void Controller::sys_load_mea_fields()
{

}

void Controller::add_field(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & field)
{
    std::unique_lock<std::shared_mutex> write_lock(m_fields_map_mutex);
    m_db_mea_map[db_name].m_mea_fields[tb_name].insert(field);
}

/**
 * 该字段是否存在
 */
bool Controller::mea_field_exist(
        const std::string & db_name,
        const std::string & tb_name,
        const std::string & field)
{
    std::shared_lock<std::shared_mutex> read_lock(m_fields_map_mutex);

    // 首先检查数据库是否存在
    auto db_it = m_db_mea_map.find(db_name);
    if (db_it == m_db_mea_map.end()) {
        // 数据库不存在
        return false;
    }

    // 然后检查表是否存在
    auto &mea_fields = db_it->second.m_mea_fields;
    auto tb_it = mea_fields.find(tb_name);
    if (tb_it == mea_fields.end()) {
        // 表不存在
        return false;
    }

    // 最后检查字段是否存在
    return tb_it->second.find(field) != tb_it->second.end();
}

/**
 * 残余数据刷盘
 */
bool Controller::is_ready_disk_write(
        const string & db_name,
        const string & tb_name,
        const string & shard_id,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            auto writer = tb_it->second.m_writer;
            if (writer)
            {
                if (writer->skip_need_flush_data_block(shard_id, field_name))
                {
                    std::cout << "---满足刷块\n";
                    // 刷跳表(传入空数据激活即可) 获取对应字段类型
                    auto fd = field_name;
                    auto tb = tb_name;
                    auto db = db_name;
                    // 利用插入数据语句,进入对应跳表激活判断逻辑引发刷块
                    m_producer_thread_pool.enqueue(&Controller::insert_thread,
                        this, system_clock::from_time_t(Shard::shard_key_to_timestamp(shard_id)), "", "", Type::DATA_STRING, fd, tb, db);
                    return true;
                }
                else
                {
                    std::cout << "---本轮不满足刷块\n";
                }
            }
        }
    }
    return false;
}

/**
 * 监控是否需要将index entry刷盘
 */
bool Controller::is_ready_index_write(
        const string & db_name,
        const string & tb_name,
        const string & shard_id,
        const string & field_name)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(tb_name);
        if (tb_it != db_it->second.m_table_map.end())
        {
            auto writer = tb_it->second.m_writer;
            if (writer)
            {
                std::cout << "---判断是否需要将index entry 刷写到磁盘\n";
                if (writer->should_flush_index(field_name))
                {
                    std::cout << "--监控线程触发写入index entry";
                    // 需要刷盘, 注册事件队列然后触发刷盘函数即可
                    writer->push_task(field_name);
                    // 线程池触发index block 刷盘
                    m_consumer_thread_pool.enqueue(&Controller::disk_write_thread, this, db_name, tb_name);
                    return true;
                }
                else
                {
                    std::cout << "---本轮不满足index entry 刷盘\n";
                }
            }
        }
    }
    return false;
}

/**
 * 开启刷盘线程
 */
bool Controller::disk_write(
        const string & db_name,
        const string & tb_name)
{
    // 从线程池拿取线程进行刷盘操作
    m_consumer_thread_pool.enqueue(&Controller::disk_write_thread, this, db_name, tb_name);
    return true;
}

void Controller::disk_write_thread(
        const string & db_name,
        const string & tb_name)
{
    std::cout << "================disk_write_thread==============" << std::endl;
    // 拿到对应Writer
    auto writer = m_map[db_name].m_table_map[tb_name].m_writer;

    if (writer)
    {
        // 调用其刷盘函数
        writer->queue_flush_disk();
    }
}